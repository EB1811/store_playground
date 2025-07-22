#include "GlobalStaticDataManager.h"
#include "Containers/Array.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Misc/AssertionMacros.h"
#include "Misc/CString.h"
#include "UObject/Field.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Npc/NpcDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Player/PlayerZDCharacter.h"
#include "store_playground/Market/Market.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/Quest/QuestManager.h"

inline void AddDialogueTableRows(TArray<struct FDialogueData>& DialogueDataArray, const UDataTable* DialogueDataTable) {
  DialogueDataArray.Empty();
  TArray<FDialogueDataTable*> Rows;
  DialogueDataTable->GetAllRows<FDialogueDataTable>("", Rows);
  for (auto* Row : Rows) {
    DialogueDataArray.Add({
        Row->DialogueChainID,
        Row->DialogueID,
        Row->DialogueType,
        Row->DialogueText,
        Row->Action,
        Row->DialogueSpeaker,
        Row->SpeakerName,
        Row->ChoicesAmount,
        Row->DialogueTags,
    });
  }
}
inline void AddDialogueTableRows(TMap<FName, FDialoguesArray>& DialogueDataMap, const UDataTable* DialogueDataTable) {
  DialogueDataMap.Empty();
  TArray<FDialogueDataTable*> Rows;
  DialogueDataTable->GetAllRows<FDialogueDataTable>("", Rows);
  for (auto* Row : Rows) {
    DialogueDataMap.FindOrAdd(Row->DialogueChainID, {});
    DialogueDataMap[Row->DialogueChainID].Dialogues.Add({
        Row->DialogueChainID,
        Row->DialogueID,
        Row->DialogueType,
        Row->DialogueText,
        Row->Action,
        Row->DialogueSpeaker,
        Row->SpeakerName,
        Row->ChoicesAmount,
        Row->DialogueTags,
    });
  }
}

AGlobalStaticDataManager::AGlobalStaticDataManager() { PrimaryActorTick.bCanEverTick = false; }

void AGlobalStaticDataManager::BeginPlay() {
  Super::BeginPlay();

  check(GenericCustomersDataTable && WantedItemTypesDataTable && UniqueNpcDataTable && PlayerMiscDialoguesTable &&
        UniqueNpcDialoguesTable && QuestDialoguesTable && CustomerDialoguesTable && MarketNpcDialoguesTable &&
        FriendlyNegDialoguesTable.DataTable && NeutralNegDialoguesTable.DataTable &&
        HostileNegDialoguesTable.DataTable && QuestChainDataTable && NpcStoreDialoguesTable && PriceEffectsDataTable &&
        ArticlesDataTable && UpgradesTable && UpgradeEffectsTable && CutsceneChainDataTable && CutsceneDataTable &&
        PopEffectsDataTable && NegotiationSkillsTable);

  InitializeCustomerData();
  InitializeNPCData();
  InitializeDialogueData();
  InitializeQuestChainsData();
  InitializeCutsceneData();
  InitializeMarketData();
  InitializeNewsData();
  InitializeUpgradesData();
}

void AGlobalStaticDataManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

TArray<struct FUniqueNpcData> AGlobalStaticDataManager::GetEligibleNpcs() const {
  const auto GameDataMap = GlobalDataManager->GetGameDataMap();
  return UniqueNpcArray.FilterByPredicate(
      [&](const FUniqueNpcData& Npc) { return EvaluateRequirementsFilter(Npc.SpawnRequirementsFilter, GameDataMap); });
}

FQuestChainData AGlobalStaticDataManager::GetQuestChainById(const FName& QuestChainID) const {
  auto* QuestChain =
      QuestChainsArray.FindByPredicate([&](const FQuestChainData& Chain) { return Chain.ID == QuestChainID; });
  check(QuestChain);

  return *QuestChain;
}

TArray<struct FQuestChainData> AGlobalStaticDataManager::GetEligibleQuestChains(
    const TArray<FName>& QuestIDs,
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
    const FName AtQuestChainID = QuestInProgress.Value;
    const int32 AtQuestChainIndex = FilteredQuestChains.IndexOfByPredicate(
        [&](const FQuestChainData& Chain) { return Chain.ID == AtQuestChainID; });
    check(AtQuestChainIndex != INDEX_NONE);

    switch (FilteredQuestChains[AtQuestChainIndex].QuestAction) {
      case EQuestAction::End: {
        break;
      }
      case EQuestAction::Continue: {
        QuestChainToStart.Add(FilteredQuestChains[AtQuestChainIndex + 1]);
        break;
      }
      case EQuestAction::SplitBranch: {
        int32 BranchesAmount = FilteredQuestChains[AtQuestChainIndex].BranchesAmount;
        int32 IgnoreNext = 0;  // Ignoring inner branches.
        for (int32 i = AtQuestChainIndex + 1; i < FilteredQuestChains.Num(); i++) {
          if (FilteredQuestChains[i].QuestChainType == EQuestChainType::Branch) {
            if (IgnoreNext > 0) {
              IgnoreNext -= 1;
              continue;
            }

            QuestChainToStart.Add(FilteredQuestChains[i]);
            BranchesAmount--;
            if (BranchesAmount <= 0) break;
          }
          if (FilteredQuestChains[i].QuestAction == EQuestAction::SplitBranch)
            IgnoreNext = FilteredQuestChains[i].BranchesAmount;
        }
        break;
      }
      default: checkNoEntry();
    }
  }

  // Evaluate last since it is the most expensive.
  const auto GameDataMap = GlobalDataManager->GetGameDataMap();
  return QuestChainToStart.FilterByPredicate([&](const FQuestChainData& Chain) {
    return EvaluateRequirementsFilter(Chain.StartRequirementsFilter, GameDataMap);
  });
}

inline TArray<struct FDialogueData> GetRandomDialogue(const TArray<struct FDialogueData>& DialogueArray) {
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
TArray<struct FDialogueData> AGlobalStaticDataManager::GetQuestDialogue(const FName& DialogueChainID) const {
  check(QuestDialoguesMap.Contains(DialogueChainID));
  return QuestDialoguesMap[DialogueChainID].Dialogues;
}

TArray<struct FDialogueData> AGlobalStaticDataManager::GetRandomPlayerMiscDialogue(
    const TArray<FName>& DialogueTags) const {
  const auto TagsContainer = StringTagsToContainer(DialogueTags);

  TArray<FDialogueData> Filtered = PlayerMiscDialogues.FilterByPredicate(
      [&](const FDialogueData& Dialogue) { return Dialogue.DialogueTags.HasAll(TagsContainer); });
  return GetRandomDialogue(Filtered);
}

TArray<struct FDialogueData> AGlobalStaticDataManager::GetRandomNpcDialogue(const TArray<FName>& DialogueChainIDs,
                                                                            const TArray<FName>& DialogueTags) const {
  const auto TagsContainer = StringTagsToContainer(DialogueTags);
  TArray<FName> FilteredIds = DialogueChainIDs.FilterByPredicate([this, TagsContainer](const FName& DialogueId) {
    return UniqueNpcDialoguesMap[DialogueId]
               .Dialogues
               .FilterByPredicate(
                   [&](const FDialogueData& Dialogue) { return Dialogue.DialogueTags.HasAll(TagsContainer); })
               .Num() > 0;
  });
  check(FilteredIds.Num() > 0);

  FName RandomDialogueChainID =
      FilteredIds.Num() == 1 ? FilteredIds[0] : FilteredIds[FMath::RandRange(0, FilteredIds.Num() - 1)];
  return UniqueNpcDialoguesMap[RandomDialogueChainID].Dialogues;
}

TArray<struct FDialogueData> AGlobalStaticDataManager::GetRandomCustomerDialogue(
    std::function<bool(const FDialogueData& Dialogue)> FilterFunc) const {
  TArray<FDialogueData> Filtered =
      !FilterFunc
          ? CustomerDialogues
          : CustomerDialogues.FilterByPredicate([&](const FDialogueData& Dialogue) { return FilterFunc(Dialogue); });

  return GetRandomDialogue(Filtered);
}
TArray<struct FDialogueData> AGlobalStaticDataManager::GetRandomMarketNpcDialogues(
    int32 Amount,
    std::function<bool(const FDialogueData& Dialogue)> FilterFunc) const {
  TArray<FDialogueData> Filtered =
      !FilterFunc
          ? MarketNpcDialogues
          : MarketNpcDialogues.FilterByPredicate([&](const FDialogueData& Dialogue) { return FilterFunc(Dialogue); });

  TArray<FDialogueData> RandDialogues = {};
  for (int32 i = 0; i < Amount; i++) {
    TArray<FDialogueData> Unique = Filtered.FilterByPredicate([&](const FDialogueData& Dialogue) {
      return !RandDialogues.ContainsByPredicate([&](const FDialogueData& D) {
        return D.DialogueChainID == Dialogue.DialogueChainID && D.DialogueID == Dialogue.DialogueID;
      });
    });
    if (Unique.Num() <= 0) break;

    RandDialogues.Append(GetRandomDialogue(Unique));
  }

  return RandDialogues;
}
TArray<struct FDialogueData> AGlobalStaticDataManager::GetRandomNpcStoreDialogue(
    std::function<bool(const FDialogueData& Dialogue)> FilterFunc) const {
  TArray<FDialogueData> Filtered =
      !FilterFunc
          ? NpcStoreDialogues
          : NpcStoreDialogues.FilterByPredicate([&](const FDialogueData& Dialogue) { return FilterFunc(Dialogue); });

  return GetRandomDialogue(Filtered);
}

TMap<ENegotiationDialogueType, FDialoguesArray> AGlobalStaticDataManager::GetRandomNegDialogueMap(
    ECustomerAttitude Attitude,
    std::function<bool(const FDialogueData& Dialogue)> FilterFunc) const {
  auto& DialoguesMap = Attitude == ECustomerAttitude::Friendly  ? FriendlyDialoguesMap
                       : Attitude == ECustomerAttitude::Neutral ? NeutralDialoguesMap
                                                                : HostileDialoguesMap;

  TMap<ENegotiationDialogueType, FDialoguesArray> RandomDialogueIndexMap;
  for (auto Type : TEnumRange<ENegotiationDialogueType>()) {
    RandomDialogueIndexMap.Add(Type, {});

    auto& Dialogues = !FilterFunc ? DialoguesMap[Type].Dialogues
                                  : DialoguesMap[Type].Dialogues.FilterByPredicate(
                                        [&](const FDialogueData& Dialogue) { return FilterFunc(Dialogue); });

    RandomDialogueIndexMap[Type].Dialogues = GetRandomDialogue(Dialogues);
  }
  return RandomDialogueIndexMap;
}

TArray<struct FPriceEffect> AGlobalStaticDataManager::GetPriceEffects(const TArray<FName>& PriceEffectIDs) const {
  return PriceEffectsArray.FilterByPredicate(
      [&](const FPriceEffect& Effect) { return PriceEffectIDs.Contains(Effect.ID); });
}
TArray<struct FPopEffect> AGlobalStaticDataManager::GetPopEffects(const TArray<FName>& PopEffectIDs) const {
  return PopEffectsArray.FilterByPredicate([&](const FPopEffect& Effect) { return PopEffectIDs.Contains(Effect.ID); });
}

TArray<struct FArticle> AGlobalStaticDataManager::GetEligibleGeneralArticles(
    const TArray<FName>& PublishedArticles) const {
  const auto GameDataMap = GlobalDataManager->GetGameDataMap();
  return ArticlesArray.FilterByPredicate([&](const FArticle& Article) {
    return (Article.bIsRepeatable || !PublishedArticles.Contains(Article.ArticleID)) && !Article.bIsSpecial &&
           !Article.bIsTriggered && EvaluateRequirementsFilter(Article.RequirementsFilter, GameDataMap);
  });
}
TArray<struct FArticle> AGlobalStaticDataManager::GetEligibleSpecialArticles(
    const TArray<FName>& PublishedArticles,
    const FGameplayTagContainer& AnyTagsContainer) const {
  const auto GameDataMap = GlobalDataManager->GetGameDataMap();

  return ArticlesArray.FilterByPredicate([&](const FArticle& Article) {
    return (Article.bIsRepeatable || !PublishedArticles.Contains(Article.ArticleID)) && Article.bIsSpecial &&
           !Article.bIsTriggered && Article.Tags.HasAny(AnyTagsContainer) &&
           EvaluateRequirementsFilter(Article.RequirementsFilter, GameDataMap);
  });
}
FArticle AGlobalStaticDataManager::GetArticle(const FName& ArticleID) const {
  return *ArticlesArray.FindByPredicate([&](const FArticle& Article) { return Article.ArticleID == ArticleID; });
}

FUpgrade AGlobalStaticDataManager::GetUpgradeById(const FName& UpgradeID) const {
  return *UpgradesArray.FindByPredicate([&](const FUpgrade& Upgrade) { return Upgrade.ID == UpgradeID; });
}
TArray<struct FUpgrade> AGlobalStaticDataManager::GetUpgradesByIds(const TArray<FName>& UpgradeIDs) const {
  return UpgradesArray.FilterByPredicate([&](const FUpgrade& Upgrade) { return UpgradeIDs.Contains(Upgrade.ID); });
}
TArray<struct FUpgrade> AGlobalStaticDataManager::GetEligibleUpgrades(EUpgradeClass UpgradeClass,
                                                                      int32 Level,
                                                                      const TArray<FName>& SelectedUpgradeIDs) const {
  const auto GameDataMap = GlobalDataManager->GetGameDataMap();
  return UpgradesArray.FilterByPredicate([&](const FUpgrade& Upgrade) {
    return Upgrade.UpgradeClass == UpgradeClass && Upgrade.Level <= Level && !SelectedUpgradeIDs.Contains(Upgrade.ID) &&
           EvaluateRequirementsFilter(Upgrade.Requirements, GameDataMap);
  });
}
TArray<struct FUpgrade> AGlobalStaticDataManager::GetUpgradesReqsNotMet(EUpgradeClass UpgradeClass,
                                                                        int32 Level,
                                                                        const TArray<FName>& SelectedUpgradeIDs) const {
  const auto GameDataMap = GlobalDataManager->GetGameDataMap();
  return UpgradesArray.FilterByPredicate([&](const FUpgrade& Upgrade) {
    return Upgrade.UpgradeClass == UpgradeClass && Upgrade.Level <= Level && !SelectedUpgradeIDs.Contains(Upgrade.ID) &&
           !EvaluateRequirementsFilter(Upgrade.Requirements, GameDataMap);
  });
}
TArray<struct FUpgradeEffect> AGlobalStaticDataManager::GetUpgradeEffectsByIds(const TArray<FName>& EffectIDs) const {
  return UpgradeEffectsArray.FilterByPredicate(
      [&](const FUpgradeEffect& Effect) { return EffectIDs.Contains(Effect.ID); });
}
TArray<struct FNegotiationSkill> AGlobalStaticDataManager::GetNegotiationSkillsByIds(
    const TArray<FName>& SkillIDs) const {
  return NegotiationSkills.FilterByPredicate(
      [&](const FNegotiationSkill& Skill) { return SkillIDs.Contains(Skill.ID); });
}

void AGlobalStaticDataManager::InitializeCustomerData() {
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
        Row->Tags,
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

void AGlobalStaticDataManager::InitializeDialogueData() {
  AddDialogueTableRows(PlayerMiscDialogues, PlayerMiscDialoguesTable);
  AddDialogueTableRows(UniqueNpcDialoguesMap, UniqueNpcDialoguesTable);
  AddDialogueTableRows(QuestDialoguesMap, QuestDialoguesTable);
  AddDialogueTableRows(CutsceneDialoguesMap, CutsceneDialoguesTable);
  AddDialogueTableRows(CustomerDialogues, CustomerDialoguesTable);
  AddDialogueTableRows(MarketNpcDialogues, MarketNpcDialoguesTable);
  AddDialogueTableRows(NpcStoreDialogues, NpcStoreDialoguesTable);

  FriendlyDialoguesMap.Empty();
  for (auto Type : TEnumRange<ENegotiationDialogueType>()) FriendlyDialoguesMap.Add(Type, {});
  TArray<FNegotiationDialoguesDataTable*> FriendlyRows;
  FriendlyNegDialoguesTable.GetRows<FNegotiationDialoguesDataTable>(FriendlyRows, TEXT("Friendly Dialogues"));
  for (FNegotiationDialoguesDataTable* Row : FriendlyRows)
    FriendlyDialoguesMap[Row->NegotiationType].Dialogues.Add({Row->DialogueChainID, Row->DialogueID, Row->DialogueType,
                                                              Row->DialogueText, Row->Action, Row->DialogueSpeaker,
                                                              FText(), Row->ChoicesAmount, Row->DialogueTags});

  NeutralDialoguesMap.Empty();
  for (auto Type : TEnumRange<ENegotiationDialogueType>()) NeutralDialoguesMap.Add(Type, {});
  TArray<FNegotiationDialoguesDataTable*> NeutralRows;
  NeutralNegDialoguesTable.GetRows<FNegotiationDialoguesDataTable>(NeutralRows, TEXT("Neutral Dialogues"));
  for (FNegotiationDialoguesDataTable* Row : NeutralRows)
    NeutralDialoguesMap[Row->NegotiationType].Dialogues.Add({Row->DialogueChainID, Row->DialogueID, Row->DialogueType,
                                                             Row->DialogueText, Row->Action, Row->DialogueSpeaker,
                                                             FText(), Row->ChoicesAmount, Row->DialogueTags});

  HostileDialoguesMap.Empty();
  for (auto Type : TEnumRange<ENegotiationDialogueType>()) HostileDialoguesMap.Add(Type, {});
  TArray<FNegotiationDialoguesDataTable*> HostileRows;
  HostileNegDialoguesTable.GetRows<FNegotiationDialoguesDataTable>(HostileRows, TEXT("Hostile Dialogues"));
  for (FNegotiationDialoguesDataTable* Row : HostileRows)
    HostileDialoguesMap[Row->NegotiationType].Dialogues.Add({Row->DialogueChainID, Row->DialogueID, Row->DialogueType,
                                                             Row->DialogueText, Row->Action, Row->DialogueSpeaker,
                                                             FText(), Row->ChoicesAmount, Row->DialogueTags});

  check(PlayerMiscDialogues.Num() > 0);
  check(UniqueNpcDialoguesMap.Num() > 0);
  check(QuestDialoguesMap.Num() > 0);
  check(CustomerDialogues.Num() > 0);
  check(MarketNpcDialogues.Num() > 0);
  check(NpcStoreDialogues.Num() > 0);
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

void AGlobalStaticDataManager::InitializeQuestChainsData() {
  QuestChainsArray.Empty();
  TArray<FQuestChainDataRow*> QuestChainRows;
  QuestChainDataTable->GetAllRows<FQuestChainDataRow>("", QuestChainRows);

  for (auto* Row : QuestChainRows) {
    QuestChainsArray.Add({
        Row->QuestID,
        Row->ID,
        Row->QuestChainType,
        Row->DialogueChainID,
        Row->CustomerAction,
        Row->ActionRelevantIDs,
        Row->PostDialogueChainID,
        Row->StartRequirementsFilter,
        Row->StartChance,
        Row->bIsRepeatable,
        Row->QuestOutcomeType,
        Row->QuestAction,
        Row->BranchesAmount,
        Row->BranchRequiredChoiceIDs,
    });
  }

  check(QuestChainsArray.Num() > 0);

  QuestChainDataTable = nullptr;
}

void AGlobalStaticDataManager::InitializeNPCData() {
  UniqueNpcArray.Empty();
  TArray<FUniqueNpcDataRow*> UniqueNpcRows;
  UniqueNpcDataTable->GetAllRows<FUniqueNpcDataRow>("", UniqueNpcRows);
  for (auto* Row : UniqueNpcRows)
    UniqueNpcArray.Add({
        Row->ID,
        Row->LinkedPopID,
        Row->NpcName,
        Row->SpawnRequirementsFilter,
        Row->SpawnWeight,
        Row->QuestIDs,
        Row->DialogueChainIDs,
        Row->NegotiationData,
        Row->AssetData,
    });

  check(UniqueNpcArray.Num() > 0);

  UniqueNpcDataTable = nullptr;
}

void AGlobalStaticDataManager::InitializeCutsceneData() {
  CutsceneChainsArray.Empty();
  TArray<FCutsceneChainDataRow*> CutsceneChainRows;
  CutsceneChainDataTable->GetAllRows<FCutsceneChainDataRow>("", CutsceneChainRows);
  for (auto* Row : CutsceneChainRows)
    CutsceneChainsArray.Add({
        Row->ID,
        Row->CutsceneID,
        Row->CutsceneChainType,
        Row->RelevantId,
        Row->RelevantLocation,
    });

  CutscenesArray.Empty();
  TArray<FCutsceneDataRow*> CutscenesRows;
  CutsceneDataTable->GetAllRows<FCutsceneDataRow>("", CutscenesRows);
  for (auto* Row : CutscenesRows)
    CutscenesArray.Add({
        Row->ID,
        Row->PlayerTagsRequirements,
        Row->Priority,
        Row->StartingNpcIDs,
        Row->StartingNpcLocations,
        Row->Tags,
    });

  check(CutsceneChainsArray.Num() > 0);
  check(CutscenesArray.Num() > 0);

  CutsceneChainDataTable = nullptr;
  CutsceneDataTable = nullptr;
}

void AGlobalStaticDataManager::InitializeMarketData() {
  PriceEffectsArray.Empty();
  TArray<FPriceEffectRow*> PriceEffectRows;
  PriceEffectsDataTable->GetAllRows<FPriceEffectRow>("", PriceEffectRows);
  for (auto* Row : PriceEffectRows)
    PriceEffectsArray.Add({
        Row->ID,
        Row->ItemEconTypes,
        Row->ItemWealthTypes,
        Row->ItemTypes,
        Row->PriceMultiPercent,
        Row->DurationType,
        Row->Duration,
        Row->PriceMultiPercentFalloff,
    });
  PopEffectsArray.Empty();
  TArray<FPopEffectRow*> PopEffectRows;
  PopEffectsDataTable->GetAllRows<FPopEffectRow>("", PopEffectRows);
  for (auto* Row : PopEffectRows)
    PopEffectsArray.Add({
        Row->ID,
        Row->PopTypes,
        Row->PopWealthTypes,
        Row->PopChangeMulti,
        Row->DurationType,
        Row->Duration,
        Row->PopChangeMultiFalloff,
    });

  check(PriceEffectsArray.Num() > 0);

  PriceEffectsDataTable = nullptr;
}

void AGlobalStaticDataManager::InitializeNewsData() {
  ArticlesArray.Empty();
  TArray<FArticleRow*> ArticleRows;
  ArticlesDataTable->GetAllRows<FArticleRow>("", ArticleRows);
  for (auto* Row : ArticleRows)
    ArticlesArray.Add({
        Row->ArticleID,
        Row->RequirementsFilter,
        Row->AppearWeight,
        Row->bIsRepeatable,
        Row->bIsSpecial,
        Row->bIsTriggered,
        Row->Size,
        Row->TextData,
        Row->AssetData,
        Row->Tags,
    });

  check(ArticlesArray.Num() > 0);
  ArticlesDataTable = nullptr;
}

void AGlobalStaticDataManager::InitializeUpgradesData() {
  UpgradesArray.Empty();
  TArray<FUpgradeRow*> UpgradesRows;
  UpgradesTable->GetAllRows<FUpgradeRow>("", UpgradesRows);
  for (auto Row : UpgradesRows)
    UpgradesArray.Add({Row->ID, Row->UpgradeClass, Row->UpgradeEffectIDs, Row->Cost, Row->Level, Row->Requirements,
                       Row->RequirementsFilterDescription, Row->TextData});

  UpgradeEffectsArray.Empty();
  TArray<FUpgradeEffectRow*> UpgradeEffectsRows;
  UpgradeEffectsTable->GetAllRows<FUpgradeEffectRow>("", UpgradeEffectsRows);
  for (auto Row : UpgradeEffectsRows)
    UpgradeEffectsArray.Add({Row->ID, Row->EffectType, Row->EffectSystem, Row->RelevantName, Row->RelevantIDs,
                             Row->RelevantValues, Row->TextData});

  NegotiationSkills.Empty();
  TArray<FNegotiationSkillRow*> NegotiationSkillsRows;
  NegotiationSkillsTable->GetAllRows<FNegotiationSkillRow>("", NegotiationSkillsRows);
  for (auto Row : NegotiationSkillsRows) NegotiationSkills.Add({Row->ID, Row->EffectType, Row->Multi, Row->TextData});

  check(UpgradesArray.Num() > 0);
  check(UpgradeEffectsArray.Num() > 0);
  check(NegotiationSkills.Num() > 0);

  UpgradesTable = nullptr;
  UpgradeEffectsTable = nullptr;
  NegotiationSkillsTable = nullptr;
}