#include "GlobalDataManager.h"
#include "Containers/Array.h"
#include "Engine/DataTable.h"
#include "Misc/CString.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Npc/NpcDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"

AGlobalDataManager::AGlobalDataManager() { PrimaryActorTick.bCanEverTick = false; }

void AGlobalDataManager::BeginPlay() {
  Super::BeginPlay();

  check(GenericCustomersDataTable && WantedItemTypesDataTable && UniqueNpcDataTable && UniqueNpcDialoguesTable &&
        QuestDialoguesTable && CustomerDialoguesTable && FriendlyNegDialoguesTable.DataTable &&
        NeutralNegDialoguesTable.DataTable && HostileNegDialoguesTable.DataTable && QuestChainDataDataTable &&
        NpcStoreTypesDataTable && NpcStoreDialoguesTable && PriceEffectsDataTable && EconEventsDataTable &&
        ArticlesDataTable);

  InitializeCustomerData();
  InitializeNPCData();
  InitializeDialogueData();
  InitializeQuestChainsData();
  InitializeNpcStoreData();
  InitializeMarketData();
  InitializeNewsData();
}

void AGlobalDataManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

template <typename T>
bool ApplyOperator(const FString& Operator, const T& Value1, const T& Value2) {
  if (Operator == TEXT("=")) return FMath::IsNearlyEqual(Value1, Value2, KINDA_SMALL_NUMBER);
  if (Operator == TEXT("!=")) return !FMath::IsNearlyEqual(Value1, Value2, KINDA_SMALL_NUMBER);
  if (Operator == TEXT("<")) return Value1 < Value2;
  if (Operator == TEXT("<=")) return Value1 <= Value2;
  if (Operator == TEXT(">")) return Value1 > Value2;
  if (Operator == TEXT(">=")) return Value1 >= Value2;

  return false;
}
template <>
bool ApplyOperator<FName>(const FString& Operator, const FName& Value1, const FName& Value2) {
  if (Operator == TEXT("=")) return Value1 == Value2;
  if (Operator == TEXT("!=")) return Value1 != Value2;

  return false;
}
template <>
bool ApplyOperator<FString>(const FString& Operator, const FString& Value1, const FString& Value2) {
  if (Operator == TEXT("=")) return Value1.Equals(Value2);
  if (Operator == TEXT("!=")) return !Value1.Equals(Value2);

  return false;
}
template <>
bool ApplyOperator<const TArray<FName>>(const FString& Operator,
                                        const TArray<FName>& Value1,
                                        const TArray<FName>& Value2) {
  // Array contains all.
  if (Operator == TEXT("contains")) {
    for (const FName& Value : Value2)
      if (!Value1.Contains(Value)) return false;
    return true;
  }

  return false;
}

bool ApplyFuncOperator(const FString& Operator, const TArray<FName>& Array, const FString& ValueString) {
  if (Operator == TEXT("contains")) {
    TArray<FString> ValueArray;
    if (ValueString.Contains("[") && ValueString.Contains(",") && ValueString.Contains("]"))
      ValueString.LeftChop(1).RightChop(1).ParseIntoArray(ValueArray, TEXT(","), true);
    if (ValueArray.Num() <= 0) ValueArray.Add(ValueString);

    TArray<FName> ValueNameArray;
    for (const FString& Value : ValueArray) ValueNameArray.Add(FName(*Value.TrimStartAndEnd()));

    return ApplyOperator<const TArray<FName>>(Operator, Array, ValueNameArray);
  }

  return ApplyOperator<int32>(Operator, Array.Num(), FCString::Atoi(*ValueString));
}

// ! Simplified inorder parser works due to no operator precedence.
// ? Move GameDataMap into data manager?
bool EvaluateRequirementsFilter(const FName& RequirementsFilter, const TMap<EReqFilterOperand, std::any>& GameDataMap) {
  if (RequirementsFilter == NAME_None) return true;
  if (GameDataMap.Num() <= 0) return true;

  FString FilterString = RequirementsFilter.ToString();
  if (FilterString.IsEmpty()) return true;

  TArray<FString> FilterExprs;
  TArray<bool> FilterExprsRes;

  TArray<FString> LogicalOperators = {TEXT("AND"), TEXT("OR")};
  TArray<FString> ExprsOperators;
  FString RemainingString = FilterString;
  int32 MaxOperators = 10;
  while ((RemainingString.Contains(TEXT("AND")) || RemainingString.Contains(TEXT("OR"))) && MaxOperators-- > 0) {
    for (const FString& Op : LogicalOperators) {
      int32 OpIndex = RemainingString.Find(Op, ESearchCase::CaseSensitive, ESearchDir::FromStart, 0);
      if (OpIndex == INDEX_NONE) continue;

      FilterExprs.Add(RemainingString.Mid(0, OpIndex).TrimStartAndEnd());
      ExprsOperators.Add(Op);

      RemainingString = RemainingString.Mid(OpIndex + Op.Len()).TrimStartAndEnd();
    }
  }
  FilterExprs.Add(RemainingString.TrimStartAndEnd());
  checkf(FilterExprs.Num() > 0, TEXT("FilterString %s does not contain any valid filter parts."), *FilterString);

  for (const FString& FilterExpr : FilterExprs) {
    FString Operand;
    EReqFilterOperand OperandE = EReqFilterOperand::Time;
    FString Operator;
    FString ValueStr;

    for (auto Op : TEnumRange<EReqFilterOperand>()) {
      FString OpStr = UEnum::GetDisplayValueAsText(Op).ToString();
      if (FilterExpr.Contains(OpStr)) {
        Operand = OpStr;
        OperandE = Op;
        break;
      }
    }
    checkf(!Operand.IsEmpty(), TEXT("FilterExpr %s does not contain a valid operand."), *FilterExpr);

    TArray<FString> ValidOperators = {TEXT("contains"), TEXT(">="), TEXT("<="), TEXT("!="),
                                      TEXT(">"),        TEXT("<"),  TEXT("=")};
    for (const FString& Op : ValidOperators) {
      if (FilterExpr.Contains(Op)) {
        Operator = Op;
        break;
      }
    }
    checkf(!Operator.IsEmpty(), TEXT("FilterExpr %s does not contain a valid operator."), *FilterExpr);

    int32 OpStart = FilterExpr.Find(Operator, ESearchCase::IgnoreCase, ESearchDir::FromStart);
    int32 OpEnd = OpStart + Operator.Len();
    ValueStr = FilterExpr.Mid(OpEnd).TrimStartAndEnd();
    TArray<FString> FuncOperators = {TEXT("contains")};
    if (FuncOperators.Contains(Operator)) {
      // e.g., contains(array, [i, j])
      ValueStr.TrimStartAndEndInline();
      int32 FuncValueIndex = ValueStr.Find(TEXT(","), ESearchCase::IgnoreCase, ESearchDir::FromStart);
      int32 FuncEndIndex = ValueStr.Find(TEXT(")"), ESearchCase::IgnoreCase, ESearchDir::FromStart);
      if (FuncValueIndex == INDEX_NONE || FuncEndIndex == INDEX_NONE)
        checkf(false, TEXT("FilterExpr %s does not contain a valid value."), *FilterExpr);

      ValueStr = ValueStr.RightChop(FuncValueIndex + 1).LeftChop(ValueStr.Len() - FuncEndIndex);
      ValueStr.TrimStartAndEndInline();
    }
    checkf(!ValueStr.IsEmpty(), TEXT("FilterExpr %s does not contain a valid value."), *FilterExpr);

    if (!GameDataMap.Contains(OperandE)) {
      checkf(false, TEXT("GameDataMap does not contain operand %s."), *Operand);
      FilterExprsRes.Add(false);
      continue;
    }

    bool EvalResult = false;
    if (GameDataMap[OperandE].type() == typeid(float))
      EvalResult = ApplyOperator(Operator, std::any_cast<float>(GameDataMap[OperandE]), FCString::Atof(*ValueStr));
    else if (GameDataMap[OperandE].type() == typeid(int32))
      EvalResult = ApplyOperator(Operator, std::any_cast<int32>(GameDataMap[OperandE]), FCString::Atoi(*ValueStr));
    else if (GameDataMap[OperandE].type() == typeid(FName))
      EvalResult = ApplyOperator(Operator, std::any_cast<FName>(GameDataMap[OperandE]), FName(*ValueStr));
    else if (GameDataMap[OperandE].type() == typeid(FString))
      EvalResult = ApplyOperator(Operator, std::any_cast<FString>(GameDataMap[OperandE]), ValueStr);
    else if (GameDataMap[OperandE].type() == typeid(TArray<FName>))
      EvalResult = ApplyFuncOperator(Operator, std::any_cast<const TArray<FName>&>(GameDataMap[OperandE]), ValueStr);
    else
      checkf(false, TEXT("FilterExpr %s does not contain a valid value."), *FilterExpr);

    FilterExprsRes.Add(EvalResult);
  }
  check(FilterExprsRes.Num() == FilterExprs.Num());

  bool Result = FilterExprsRes[0];
  for (int32 i = 0; i < ExprsOperators.Num(); i++)
    if (ExprsOperators[i] == TEXT("AND"))
      Result = Result && FilterExprsRes[i + 1];
    else if (ExprsOperators[i] == TEXT("OR"))
      Result = Result || FilterExprsRes[i + 1];

  return Result;
}

// Temp: Only money for now.
TArray<struct FUniqueNpcData> AGlobalDataManager::GetEligibleNpcs(
    const TMap<EReqFilterOperand, std::any>& GameDataMap) const {
  return UniqueNpcArray.FilterByPredicate(
      [&](const FUniqueNpcData& Npc) { return EvaluateRequirementsFilter(Npc.SpawnRequirementsFilter, GameDataMap); });
}

TArray<struct FQuestChainData> AGlobalDataManager::GetEligibleQuestChains(
    const TArray<FName>& QuestIDs,
    const TMap<EReqFilterOperand, std::any>& GameDataMap,
    TArray<FName> CompletedQuestIDs,
    TMap<FName, FName> PrevChainCompletedMap) const {
  auto FilteredQuestChains = QuestChainsArray.FilterByPredicate([&](const FQuestChainData& Chain) {
    return QuestIDs.Contains(Chain.QuestID) && (Chain.bIsRepeatable || !CompletedQuestIDs.Contains(Chain.QuestID));
  });

  // Quest in progress tracking.
  TArray<struct FQuestChainData> QuestChainToStart;  // For each quest.
  for (const FQuestChainData& Chain : FilteredQuestChains)
    if (!PrevChainCompletedMap.Contains(Chain.QuestID) &&
        !QuestChainToStart.ContainsByPredicate(
            [&](const FQuestChainData& ChainToStart) { return ChainToStart.QuestID == Chain.QuestID; }))
      QuestChainToStart.Add(Chain);
  for (const auto& QuestInProgress : PrevChainCompletedMap) {
    FName AtQuestChainID = QuestInProgress.Value;
    int32 AtQuestChainIndex = FilteredQuestChains.IndexOfByPredicate(
        [&](const FQuestChainData& Chain) { return Chain.QuestChainID == AtQuestChainID; });
    check(AtQuestChainIndex != INDEX_NONE);

    if (FilteredQuestChains[AtQuestChainIndex].QuestAction == EQuestAction::End) continue;
    QuestChainToStart.Add(FilteredQuestChains[AtQuestChainIndex + 1]);
  }

  // Evaluate last since it is the most expensive.
  return QuestChainToStart.FilterByPredicate([&](const FQuestChainData& Chain) {
    return EvaluateRequirementsFilter(Chain.StartRequirementsFilter, GameDataMap);
  });
}

TArray<struct FDialogueData> AGlobalDataManager::GetQuestDialogue(const FName& DialogueChainID) const {
  check(QuestDialoguesMap.Contains(DialogueChainID));
  return QuestDialoguesMap[DialogueChainID].Dialogues;
}

TArray<struct FDialogueData> AGlobalDataManager::GetRandomNpcDialogue(const TArray<FName>& DialogueChainIDs) const {
  FName RandomDialogueChainID = DialogueChainIDs[FMath::RandRange(0, DialogueChainIDs.Num() - 1)];

  return UniqueNpcDialoguesMap[RandomDialogueChainID].Dialogues;
}

TArray<struct FDialogueData> GetRandomDialogue(const TArray<struct FDialogueData>& DialogueArray) {
  TArray<struct FDialogueData> RandomDialogue;

  int32 MapSize = DialogueArray.Num();
  int32 RandomIndex = FMath::RandRange(0, MapSize - 1);
  for (int32 i = RandomIndex - 1; i >= 0; i--) {
    if (DialogueArray[i].DialogueChainID != DialogueArray[RandomIndex].DialogueChainID) break;
    RandomIndex = i;
  }

  for (int32 i = RandomIndex; i < MapSize; i++) {
    if (DialogueArray[i].DialogueChainID != DialogueArray[RandomIndex].DialogueChainID) break;
    RandomDialogue.Add(DialogueArray[i]);
  }

  return RandomDialogue;
}

TArray<struct FDialogueData> AGlobalDataManager::GetRandomCustomerDialogue() const {
  return GetRandomDialogue(CustomerDialogues);
}

TArray<struct FDialogueData> AGlobalDataManager::GetRandomNpcStoreDialogue() const {
  return GetRandomDialogue(NpcStoreDialogues);
}

TMap<ENegotiationDialogueType, FDialoguesArray> AGlobalDataManager::GetRandomNegDialogueMap(
    ECustomerAttitude Attitude) const {
  auto& DialoguesMap = Attitude == ECustomerAttitude::Friendly  ? FriendlyDialoguesMap
                       : Attitude == ECustomerAttitude::Neutral ? NeutralDialoguesMap
                                                                : HostileDialoguesMap;

  TMap<ENegotiationDialogueType, FDialoguesArray> RandomDialogueIndexMap;
  for (auto Type : TEnumRange<ENegotiationDialogueType>()) {
    RandomDialogueIndexMap.Add(Type, {});

    auto& Dialogues = DialoguesMap[Type].Dialogues;
    RandomDialogueIndexMap[Type].Dialogues = GetRandomDialogue(Dialogues);
  }
  return RandomDialogueIndexMap;
}

TArray<struct FEconEvent> AGlobalDataManager::GetEligibleEconEvents(
    const TMap<EReqFilterOperand, std::any>& GameDataMap,
    const TArray<FName>& OccurredEconEvents) const {
  return EconEventsArray.FilterByPredicate([&](const FEconEvent& Event) {
    return (Event.bIsRepeatable || !OccurredEconEvents.Contains(Event.EconEventID)) &&
           EvaluateRequirementsFilter(Event.RequirementsFilter, GameDataMap);
  });
}

TArray<struct FPriceEffect> AGlobalDataManager::GetPriceEffects(const TArray<FName>& PriceEffectIDs) const {
  return PriceEffectsArray.FilterByPredicate(
      [&](const FPriceEffect& Effect) { return PriceEffectIDs.Contains(Effect.PriceEffectID); });
}

TArray<struct FArticle> AGlobalDataManager::GetEligibleArticles(const TMap<EReqFilterOperand, std::any>& GameDataMap,
                                                                const TArray<FName>& PublishedArticles) const {
  return ArticlesArray.FilterByPredicate([&](const FArticle& Article) {
    return (Article.bIsRepeatable || !PublishedArticles.Contains(Article.ArticleID)) &&
           EvaluateRequirementsFilter(Article.RequirementsFilter, GameDataMap);
  });
}

FArticle AGlobalDataManager::GetArticle(const FName& ArticleID) const {
  return *ArticlesArray.FindByPredicate([&](const FArticle& Article) { return Article.ArticleID == ArticleID; });
}

void AGlobalDataManager::InitializeCustomerData() {
  GenericCustomersArray.Empty();
  TArray<FCustomerDataRow*> GenericCustomersRows;
  GenericCustomersDataTable->GetAllRows<FCustomerDataRow>("", GenericCustomersRows);
  for (auto* Row : GenericCustomersRows)
    GenericCustomersArray.Add({
        Row->CustomerID,
        Row->LinkedPopID,
        Row->CustomerName,
        Row->InitAttitude,
        Row->AssetData,
    });

  WantedItemTypesArray.Empty();
  TArray<FWantedItemTypeRow*> WantedItemTypesRows;
  WantedItemTypesDataTable->GetAllRows<FWantedItemTypeRow>("", WantedItemTypesRows);
  for (auto* Row : WantedItemTypesRows)
    WantedItemTypesArray.Add({
        Row->WantedItemTypeID,
        Row->WantedItemTypeName,
        Row->ItemType,
        Row->ItemEconType,
    });

  check(GenericCustomersArray.Num() > 0);
  check(WantedItemTypesArray.Num() > 0);

  GenericCustomersDataTable = nullptr;
  WantedItemTypesDataTable = nullptr;
}

void AGlobalDataManager::InitializeDialogueData() {
  UniqueNpcDialoguesMap.Empty();
  TArray<FDialogueDataTable*> UniqueNpcDialoguesRows;
  UniqueNpcDialoguesTable->GetAllRows<FDialogueDataTable>("", UniqueNpcDialoguesRows);
  for (auto* Row : UniqueNpcDialoguesRows) {
    UniqueNpcDialoguesMap.FindOrAdd(Row->DialogueChainID, {});
    UniqueNpcDialoguesMap[Row->DialogueChainID].Dialogues.Add({
        Row->DialogueChainID,
        Row->DialogueID,
        Row->DialogueType,
        Row->DialogueText,
        Row->Action,
        Row->DialogueSpeaker,
        Row->ChoicesAmount,
    });
  }
  QuestDialoguesMap.Empty();
  TArray<FDialogueDataTable*> QuestDialoguesRows;
  QuestDialoguesTable->GetAllRows<FDialogueDataTable>("", QuestDialoguesRows);
  for (auto* Row : QuestDialoguesRows) {
    QuestDialoguesMap.FindOrAdd(Row->DialogueChainID, {});
    QuestDialoguesMap[Row->DialogueChainID].Dialogues.Add({
        Row->DialogueChainID,
        Row->DialogueID,
        Row->DialogueType,
        Row->DialogueText,
        Row->Action,
        Row->DialogueSpeaker,
        Row->ChoicesAmount,
    });
  }

  CustomerDialogues.Empty();
  TArray<FDialogueDataTable*> CustomerDialoguesRows;
  CustomerDialoguesTable->GetAllRows<FDialogueDataTable>("", CustomerDialoguesRows);
  for (auto* Row : CustomerDialoguesRows)
    CustomerDialogues.Add({
        Row->DialogueChainID,
        Row->DialogueID,
        Row->DialogueType,
        Row->DialogueText,
        Row->Action,
        Row->DialogueSpeaker,
        Row->ChoicesAmount,
    });

  FriendlyDialoguesMap.Empty();
  for (auto Type : TEnumRange<ENegotiationDialogueType>()) FriendlyDialoguesMap.Add(Type, {});
  TArray<FNegotiationDialoguesDataTable*> FriendlyRows;
  FriendlyNegDialoguesTable.GetRows<FNegotiationDialoguesDataTable>(FriendlyRows, TEXT("Friendly Dialogues"));
  for (FNegotiationDialoguesDataTable* Row : FriendlyRows)
    FriendlyDialoguesMap[Row->NegotiationType].Dialogues.Add({Row->DialogueChainID, Row->DialogueID, Row->DialogueType,
                                                              Row->DialogueText, Row->Action, Row->DialogueSpeaker,
                                                              Row->ChoicesAmount});

  NeutralDialoguesMap.Empty();
  for (auto Type : TEnumRange<ENegotiationDialogueType>()) NeutralDialoguesMap.Add(Type, {});
  TArray<FNegotiationDialoguesDataTable*> NeutralRows;
  NeutralNegDialoguesTable.GetRows<FNegotiationDialoguesDataTable>(NeutralRows, TEXT("Neutral Dialogues"));
  for (FNegotiationDialoguesDataTable* Row : NeutralRows)
    NeutralDialoguesMap[Row->NegotiationType].Dialogues.Add({Row->DialogueChainID, Row->DialogueID, Row->DialogueType,
                                                             Row->DialogueText, Row->Action, Row->DialogueSpeaker,
                                                             Row->ChoicesAmount});

  HostileDialoguesMap.Empty();
  for (auto Type : TEnumRange<ENegotiationDialogueType>()) HostileDialoguesMap.Add(Type, {});
  TArray<FNegotiationDialoguesDataTable*> HostileRows;
  HostileNegDialoguesTable.GetRows<FNegotiationDialoguesDataTable>(HostileRows, TEXT("Hostile Dialogues"));
  for (FNegotiationDialoguesDataTable* Row : HostileRows)
    HostileDialoguesMap[Row->NegotiationType].Dialogues.Add({Row->DialogueChainID, Row->DialogueID, Row->DialogueType,
                                                             Row->DialogueText, Row->Action, Row->DialogueSpeaker,
                                                             Row->ChoicesAmount});

  NpcStoreDialogues.Empty();
  TArray<FDialogueDataTable*> NpcStoreDialoguesRows;
  NpcStoreDialoguesTable->GetAllRows<FDialogueDataTable>("", NpcStoreDialoguesRows);
  for (auto* Row : NpcStoreDialoguesRows)
    NpcStoreDialogues.Add({
        Row->DialogueChainID,
        Row->DialogueID,
        Row->DialogueType,
        Row->DialogueText,
        Row->Action,
        Row->DialogueSpeaker,
        Row->ChoicesAmount,
    });

  check(NpcStoreDialogues.Num() > 0);
  check(UniqueNpcDialoguesMap.Num() > 0);
  check(CustomerDialogues.Num() > 0);
  // for (auto Type : TEnumRange<ENegotiationDialogueType>())
  //   check(FriendlyDialoguesMap[Type].Dialogues.Num() > 0 && NeutralDialoguesMap[Type].Dialogues.Num() > 0 &&
  //         HostileDialoguesMap[Type].Dialogues.Num() > 0);

  UniqueNpcDialoguesTable = nullptr;
  QuestDialoguesTable = nullptr;
  CustomerDialoguesTable = nullptr;
  FriendlyNegDialoguesTable.DataTable = nullptr;
  NeutralNegDialoguesTable.DataTable = nullptr;
  HostileNegDialoguesTable.DataTable = nullptr;
  NpcStoreDialoguesTable = nullptr;
}

void AGlobalDataManager::InitializeQuestChainsData() {
  QuestChainsArray.Empty();
  TArray<FQuestChainDataRow*> QuestChainRows;
  QuestChainDataDataTable->GetAllRows<FQuestChainDataRow>("", QuestChainRows);

  for (auto* Row : QuestChainRows) {
    QuestChainsArray.Add({
        Row->QuestID,
        Row->QuestChainID,
        Row->QuestChainType,
        Row->DialogueChainID,
        Row->CustomerAction,
        Row->ActionRelevantIDs,
        Row->PostDialogueChainID,
        Row->StartRequirementsFilter,
        Row->StartChance,
        Row->bIsRepeatable,
        Row->QuestAction,
        Row->BranchesAmount,
        Row->BranchRequiredChoiceIDs,
    });
  }

  check(QuestChainsArray.Num() > 0);

  QuestChainDataDataTable = nullptr;
}

void AGlobalDataManager::InitializeNPCData() {
  UniqueNpcArray.Empty();
  TArray<FUniqueNpcDataRow*> UniqueNpcRows;
  UniqueNpcDataTable->GetAllRows<FUniqueNpcDataRow>("", UniqueNpcRows);
  for (auto* Row : UniqueNpcRows)
    UniqueNpcArray.Add({
        Row->NpcID,
        Row->LinkedPopID,
        Row->NpcName,
        Row->SpawnRequirementsFilter,
        Row->SpawnChanceWeight,
        Row->QuestIDs,
        Row->DialogueChainIDs,
        Row->NegotiationData,
        Row->AssetData,
    });

  check(UniqueNpcArray.Num() > 0);

  UniqueNpcDataTable = nullptr;
}

void AGlobalDataManager::InitializeNpcStoreData() {
  NpcStoreTypesArray.Empty();
  TArray<FNpcStoreTypeRow*> NpcStoreTypesRows;
  NpcStoreTypesDataTable->GetAllRows<FNpcStoreTypeRow>("", NpcStoreTypesRows);
  for (auto* Row : NpcStoreTypesRows)
    NpcStoreTypesArray.Add({
        Row->NpcStoreTypeID,
        Row->StoreTypeName,
        Row->StoreSpawnWeight,
        Row->StockCountRange,
        Row->StorePriceMarkup,
        Row->ItemTypeWeightMap,
        Row->ItemEconTypeWeightMap,
    });

  check(NpcStoreTypesArray.Num() > 0);

  NpcStoreTypesDataTable = nullptr;
}

void AGlobalDataManager::InitializeMarketData() {
  PriceEffectsArray.Empty();
  TArray<FPriceEffectRow*> PriceEffectRows;
  PriceEffectsDataTable->GetAllRows<FPriceEffectRow>("", PriceEffectRows);
  for (auto* Row : PriceEffectRows)
    PriceEffectsArray.Add({
        Row->PriceEffectID,
        Row->ItemEconTypes,
        Row->ItemWealthTypes,
        Row->ItemTypes,
        Row->PriceMultiPercent,
        Row->Duration,
        Row->PriceMultiPercentFalloff,
    });

  EconEventsArray.Empty();
  TArray<FEconEventRow*> EconEventRows;
  EconEventsDataTable->GetAllRows<FEconEventRow>("", EconEventRows);
  for (auto* Row : EconEventRows)
    EconEventsArray.Add({
        Row->EconEventID,
        Row->RequirementsFilter,
        Row->StartChance,
        Row->bIsRepeatable,
        Row->PriceEffectIDs,
        Row->ArticleID,
        Row->TextData,
        Row->AssetData,
    });

  check(PriceEffectsArray.Num() > 0);
  check(EconEventsArray.Num() > 0);

  PriceEffectsDataTable = nullptr;
  EconEventsDataTable = nullptr;
}

void AGlobalDataManager::InitializeNewsData() {
  ArticlesArray.Empty();
  TArray<FArticleRow*> ArticleRows;
  ArticlesDataTable->GetAllRows<FArticleRow>("", ArticleRows);
  for (auto* Row : ArticleRows)
    ArticlesArray.Add({
        Row->ArticleID,
        Row->RequirementsFilter,
        Row->AppearWeight,
        Row->bIsRepeatable,
        Row->Size,
        Row->TextData,
        Row->AssetData,
    });

  check(ArticlesArray.Num() > 0);
  ArticlesDataTable = nullptr;
}