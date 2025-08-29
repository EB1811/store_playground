#include "GlobalDataManager.h"
#include "Containers/Array.h"
#include "Engine/DataTable.h"
#include "Misc/AssertionMacros.h"
#include "Misc/CString.h"
#include "UObject/Field.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Tags/TagsComponent.h"
#include "store_playground/Npc/NpcDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Player/PlayerZDCharacter.h"
#include "store_playground/Market/Market.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/Quest/QuestManager.h"

AGlobalDataManager::AGlobalDataManager() {
  PrimaryActorTick.bCanEverTick = false;

  Upgradeable.ChangeData = [this](FName DataName, const TArray<FName>& FilterIds,
                                  const TMap<FName, float>& ParamValues) {
    ChangeData(DataName, FilterIds, ParamValues);
  };
}

void AGlobalDataManager::BeginPlay() {
  Super::BeginPlay();

  check(NpcStoreTypesDataTable && EconEventsDataTable && EconEventAbilitiesTable);

  InitializeNpcStoreData();
  InitializeMarketData();
  InitializeUpgradesData();
}

void AGlobalDataManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

bool EvaluatePlayerTagsRequirements(const FGameplayTagContainer& RequiredTags,
                                    const class UTagsComponent* PlayerTagsC) {
  if (RequiredTags.IsEmpty()) return true;

  if (PlayerTagsC->GetAllTags().HasAllExact(RequiredTags)) return true;
  return false;
}

template <typename T>
inline bool ApplyOperator(const FString& Operator, const T& Value1, const T& Value2) {
  if (Operator == TEXT("=")) return FMath::IsNearlyEqual(Value1, Value2, KINDA_SMALL_NUMBER);
  if (Operator == TEXT("!=")) return !FMath::IsNearlyEqual(Value1, Value2, KINDA_SMALL_NUMBER);
  if (Operator == TEXT("<")) return Value1 < Value2;
  if (Operator == TEXT("<=")) return Value1 <= Value2;
  if (Operator == TEXT(">")) return Value1 > Value2;
  if (Operator == TEXT(">=")) return Value1 >= Value2;

  return false;
}
template <>
inline bool ApplyOperator<FName>(const FString& Operator, const FName& Value1, const FName& Value2) {
  if (Operator == TEXT("=")) return Value1 == Value2;
  if (Operator == TEXT("!=")) return Value1 != Value2;

  return false;
}
template <>
inline bool ApplyOperator<FString>(const FString& Operator, const FString& Value1, const FString& Value2) {
  if (Operator == TEXT("=")) return Value1.Equals(Value2);
  if (Operator == TEXT("!=")) return !Value1.Equals(Value2);

  return false;
}
template <>
inline bool ApplyOperator<const TArray<FName>>(const FString& Operator,
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
inline bool ApplyFuncOperator(const FString& Operator, const TArray<FName>& Array, const FString& ValueString) {
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
inline bool ApplyFuncOperator(const FString& Operator,
                              const FGameplayTagContainer& Container,
                              const FString& ValueString) {
  if (Operator == TEXT("contains")) {
    TArray<FString> ValueArray;
    if (ValueString.Contains("[") && ValueString.Contains(",") && ValueString.Contains("]"))
      ValueString.LeftChop(1).RightChop(1).ParseIntoArray(ValueArray, TEXT(","), true);
    if (ValueArray.Num() <= 0) ValueArray.Add(ValueString);

    FGameplayTagContainer ValueNameContainer;
    for (const FString& Value : ValueArray) {
      auto Tag = FGameplayTag::RequestGameplayTag(FName(*Value.TrimStartAndEnd()));
      check(Tag.IsValid());
      ValueNameContainer.AddTag(Tag);
    }

    if (Operator == TEXT("contains")) return Container.HasAll(ValueNameContainer);
    return false;
  }

  return ApplyOperator<int32>(Operator, Container.Num(), FCString::Atoi(*ValueString));
}

// ! Simplified inorder parser works due to no operator precedence.
bool EvaluateRequirementsFilter(const FName& RequirementsFilter, const TMap<EReqFilterOperand, std::any>& GameDataMap) {
  if (RequirementsFilter.IsNone()) return true;
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
    else if (GameDataMap[OperandE].type() == typeid(FGameplayTagContainer))
      EvalResult =
          ApplyFuncOperator(Operator, std::any_cast<const FGameplayTagContainer&>(GameDataMap[OperandE]), ValueStr);
    else checkf(false, TEXT("FilterExpr %s does not contain a valid value."), *FilterExpr);

    FilterExprsRes.Add(EvalResult);
  }
  check(FilterExprsRes.Num() == FilterExprs.Num());

  bool Result = FilterExprsRes[0];
  for (int32 i = 0; i < ExprsOperators.Num(); i++)
    if (ExprsOperators[i] == TEXT("AND")) Result = Result && FilterExprsRes[i + 1];
    else if (ExprsOperators[i] == TEXT("OR")) Result = Result || FilterExprsRes[i + 1];

  // UE_LOG(LogTemp, Log, TEXT("RequirementsFilter %s evaluated to %s."), *RequirementsFilter.ToString(),
  //        Result ? TEXT("true") : TEXT("false"));

  return Result;
}

const TMap<EReqFilterOperand, std::any> AGlobalDataManager::GetGameDataMap() const {
  check(PlayerCharacter && UpgradeManager && DayManager && Store && QuestManager && Market && NewsGen);

  const FGameplayTagContainer PlayerTags = PlayerCharacter->PlayerTagsComponent->GetAllTags();
  const TArray<FName> InventoryItems = FormIdList<TObjectPtr<UItemBase>>(
      PlayerCharacter->PlayerInventoryComponent->ItemsArray, [](const UItemBase* Item) { return Item->ItemID; });
  TArray<FName> MadeDialogueChoices = {};
  for (const auto& Choice : QuestManager->QuestInProgressMap) MadeDialogueChoices.Append(Choice.Value.ChoicesMade);
  TArray<FName> RecentEconEvents = {};
  Market->RecentEconEventsMap.GetKeys(RecentEconEvents);

  const TMap<EReqFilterOperand, std::any> GameDataMap = {
      {EReqFilterOperand::Time, DayManager->CurrentDay},
      {EReqFilterOperand::Money, Store->Money},
      {EReqFilterOperand::PlayerTags, PlayerTags},
      {EReqFilterOperand::Inventory, InventoryItems},
      {EReqFilterOperand::QuestsCompleted, QuestManager->QuestsCompleted},
      {EReqFilterOperand::MadeDialogueChoices, MadeDialogueChoices},
      {EReqFilterOperand::RecentEconEvents, RecentEconEvents},
      {EReqFilterOperand::PublishedArticles, NewsGen->PublishedArticles},
      {EReqFilterOperand::SelectedUpgradeIDs, UpgradeManager->SelectedUpgradeIDs},
  };

  return GameDataMap;
}

TArray<struct FEconEvent> AGlobalDataManager::GetEligibleEconEvents(const TArray<FName>& OccurredEconEvents) const {
  const auto GameDataMap = GetGameDataMap();
  return EconEventsArray.FilterByPredicate([&](const FEconEvent& Event) {
    return (Event.bIsRepeatable || !OccurredEconEvents.Contains(Event.ID)) && Event.bIsUnlocked &&
           EvaluateRequirementsFilter(Event.RequirementsFilter, GameDataMap);
  });
}
TArray<struct FEconEvent> AGlobalDataManager::GetEconEventsByIds(const TArray<FName>& EventIDs) const {
  return EconEventsArray.FilterByPredicate([&](const FEconEvent& Event) { return EventIDs.Contains(Event.ID); });
}

auto AGlobalDataManager::GetEconEventAbilitiesByIds(const TArray<FName>& AbilityIDs) const
    -> TArray<struct FEconEventAbility> {
  return EconEventAbilitiesArray.FilterByPredicate(
      [&](const FEconEventAbility& Ability) { return AbilityIDs.Contains(Ability.ID); });
}

void AGlobalDataManager::ChangeData(const FName DataName,
                                    const TArray<FName>& FilterIds,
                                    const TMap<FName, float>& ParamValues) {
  auto ArrayProp = CastField<FArrayProperty>(this->GetClass()->FindPropertyByName(DataName));
  check(ArrayProp);

  auto ArrayPtr = ArrayProp->ContainerPtrToValuePtr<void*>(this);
  FScriptArrayHelper ArrayHelper(ArrayProp, ArrayPtr);
  for (int32 i = 0; i < ArrayHelper.Num(); i++) {
    auto StructProp = CastField<FStructProperty>(ArrayProp->Inner);
    auto StructPtr = ArrayHelper.GetRawPtr(i);

    if (FilterIds.Num() > 0) {
      auto IdProp = StructProp->Struct->FindPropertyByName("ID");
      check(IdProp);
      FName* IdValuePtr = IdProp->ContainerPtrToValuePtr<FName>(StructPtr);
      UE_LOG(LogTemp, Log, TEXT("IdValuePtr: %s"), *IdValuePtr->ToString());
      if (!FilterIds.Contains(*IdValuePtr)) continue;
    }

    for (const auto& ParamPair : ParamValues)
      AddToStructPropertyValue(StructProp, StructPtr, ParamPair.Key, ParamPair.Value);
  }
}

void AGlobalDataManager::InitializeNpcStoreData() {
  NpcStoreTypesArray.Empty();
  TArray<FNpcStoreTypeRow*> NpcStoreTypesRows;
  NpcStoreTypesDataTable->GetAllRows<FNpcStoreTypeRow>("", NpcStoreTypesRows);
  for (auto* Row : NpcStoreTypesRows)
    NpcStoreTypesArray.Add({
        Row->ID,
        Row->StoreTypeName,
        Row->StoreSpawnWeight,
        Row->StockCountRange,
        Row->StoreMarkup,
        Row->StoreMarkupVariation,
        Row->ItemTypeWeightMap,
        Row->ItemEconTypeWeightMap,
        Row->DisplayName,
        Row->AssetData,
    });

  check(NpcStoreTypesArray.Num() > 0);

  NpcStoreTypesDataTable = nullptr;
}

void AGlobalDataManager::InitializeMarketData() {
  EconEventsArray.Empty();
  TArray<FEconEventRow*> EconEventRows;
  EconEventsDataTable->GetAllRows<FEconEventRow>("", EconEventRows);
  for (auto* Row : EconEventRows)
    EconEventsArray.Add({
        Row->ID,
        Row->Severity,
        !Row->bIsUnlockable,  // bIsUnlocked = !bIsUnlockable
        Row->RequirementsFilter,
        Row->StartChance,
        Row->bIsRepeatable,
        Row->PriceEffectIDs,
        Row->PopEffectIDs,
        Row->ArticleID,
        Row->TextData,
        Row->AssetData,
        Row->Tags,
    });
  check(EconEventsArray.Num() > 0);

  EconEventsDataTable = nullptr;
}

void AGlobalDataManager::InitializeUpgradesData() {
  EconEventAbilitiesArray.Empty();
  TArray<FEconEventAbilityRow*> EconEventAbilitiesRows;
  EconEventAbilitiesTable->GetAllRows<FEconEventAbilityRow>("", EconEventAbilitiesRows);
  for (auto Row : EconEventAbilitiesRows)
    EconEventAbilitiesArray.Add({
        Row->ID,
        Row->EconEventId,
        Row->Cost,
        Row->Duration,
        Row->Cooldown,
        Row->UpgradeClass,
        Row->TextData,
    });
  check(EconEventAbilitiesArray.Num() > 0);

  EconEventAbilitiesTable = nullptr;
}