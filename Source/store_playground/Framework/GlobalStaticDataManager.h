// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <any>
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Npc/NpcDataStructs.h"
#include "store_playground/AI/CustomerDataStructs.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "store_playground/NewsGen/NewsGenDataStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "store_playground/Cutscene/CutsceneStructs.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "GlobalStaticDataManager.generated.h"

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AGlobalStaticDataManager : public AInfo {
  GENERATED_BODY()

public:
  AGlobalStaticDataManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "GameDataMap")
  const class AGlobalDataManager* GlobalDataManager;  // * To access EvaluateRequirementsFilter and GameDataMap.

  // Data Tables - Static configuration data loaded from tables
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> GenericCustomersDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> WantedItemTypesDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> PlayerMiscDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> UniqueNpcDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> QuestDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> CutsceneDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> CustomerDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> MarketNpcDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  struct FDataTableCategoryHandle FriendlyNegDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  struct FDataTableCategoryHandle NeutralNegDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  struct FDataTableCategoryHandle HostileNegDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> UniqueNpcDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> QuestChainDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> CutsceneChainDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> CutsceneDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> NpcStoreDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> PriceEffectsDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> PopEffectsDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> ArticlesDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> UpgradesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> UpgradeEffectsTable;

  // Arrays and Maps to hold loaded data
  UPROPERTY(EditAnywhere, Category = "GenericCustomer")
  TArray<struct FGenericCustomerData> GenericCustomersArray;
  UPROPERTY(EditAnywhere, Category = "GenericCustomer")
  TArray<struct FWantedItemType> WantedItemTypesArray;

  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TArray<struct FDialogueData> PlayerMiscDialogues;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TMap<FName, FDialoguesArray> UniqueNpcDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TMap<FName, FDialoguesArray> QuestDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TMap<FName, FDialoguesArray> CutsceneDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TArray<struct FDialogueData> CustomerDialogues;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TArray<struct FDialogueData> MarketNpcDialogues;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TArray<struct FDialogueData> NpcStoreDialogues;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TMap<ENegotiationDialogueType, FDialoguesArray> FriendlyDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TMap<ENegotiationDialogueType, FDialoguesArray> NeutralDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TMap<ENegotiationDialogueType, FDialoguesArray> HostileDialoguesMap;

  UPROPERTY(EditAnywhere, Category = "UniqueNpc")
  TArray<struct FUniqueNpcData> UniqueNpcArray;
  UPROPERTY(EditAnywhere, Category = "UniqueNpc")
  TArray<struct FQuestChainData> QuestChainsArray;

  UPROPERTY(EditAnywhere, Category = "Cutscene")
  TArray<struct FCutsceneChainData> CutsceneChainsArray;
  UPROPERTY(EditAnywhere, Category = "Cutscene")
  TArray<struct FCutsceneData> CutscenesArray;

  UPROPERTY(EditAnywhere, Category = "Market")
  TArray<struct FPriceEffect> PriceEffectsArray;
  UPROPERTY(EditAnywhere, Category = "Market")
  TArray<struct FPopEffect> PopEffectsArray;

  UPROPERTY(EditAnywhere, Category = "News")
  TArray<struct FArticle> ArticlesArray;
  UPROPERTY(EditAnywhere, Category = "Upgrades")
  TArray<struct FUpgrade> UpgradesArray;
  UPROPERTY(EditAnywhere, Category = "Upgrades")
  TArray<struct FUpgradeEffect> UpgradeEffectsArray;

  // Data access methods
  TArray<struct FUniqueNpcData> GetEligibleNpcs() const;

  struct FQuestChainData GetQuestChainById(const FName& QuestChainID) const;
  TArray<struct FQuestChainData> GetEligibleQuestChains(const TArray<FName>& QuestIDs,
                                                        TArray<FName> CompletedQuestIDs,
                                                        TMap<FName, FName> PrevChainCompletedMap) const;

  TArray<struct FDialogueData> GetQuestDialogue(const FName& DialogueChainID) const;
  TArray<struct FDialogueData> GetRandomPlayerMiscDialogue(const TArray<FName>& DialogueTags) const;
  TArray<struct FDialogueData> GetRandomNpcDialogue(const TArray<FName>& DialogueChainIDs) const;
  TArray<struct FDialogueData> GetRandomCustomerDialogue(
      std::function<bool(const FDialogueData& Dialogue)> FilterFunc = nullptr) const;
  TArray<struct FDialogueData> GetRandomMarketNpcDialogue(
      std::function<bool(const FDialogueData& Dialogue)> FilterFunc = nullptr) const;
  TArray<struct FDialogueData> GetRandomNpcStoreDialogue(
      std::function<bool(const FDialogueData& Dialogue)> FilterFunc = nullptr) const;
  TMap<ENegotiationDialogueType, FDialoguesArray> GetRandomNegDialogueMap(
      ECustomerAttitude Attitude = ECustomerAttitude::Neutral,
      std::function<bool(const FDialogueData& Dialogue)> FilterFunc = nullptr) const;

  TArray<struct FPriceEffect> GetPriceEffects(const TArray<FName>& PriceEffectIDs) const;
  TArray<struct FPopEffect> GetPopEffects(const TArray<FName>& PopEffectIDs) const;

  TArray<struct FArticle> GetEligibleGeneralArticles(const TArray<FName>& PublishedArticles) const;
  TArray<struct FArticle> GetEligibleSpecialArticles(const TArray<FName>& PublishedArticles,
                                                     const FGameplayTagContainer& AnyTagsContainer) const;
  FArticle GetArticle(const FName& ArticleID) const;

  FUpgrade GetUpgradeById(const FName& UpgradeID) const;
  TArray<struct FUpgrade> GetUpgradesByIds(const TArray<FName>& UpgradeIDs) const;
  TArray<struct FUpgrade> GetEligibleUpgrades(EUpgradeClass UpgradeClass,
                                              int32 Level,
                                              const TArray<FName>& SelectedUpgradeIDs) const;
  TArray<struct FUpgrade> GetUpgradesReqsNotMet(EUpgradeClass UpgradeClass,
                                                int32 Level,
                                                const TArray<FName>& SelectedUpgradeIDs) const;
  TArray<struct FUpgradeEffect> GetUpgradeEffectsByIds(const TArray<FName>& EffectIDs) const;

  // Data initialization methods
  void InitializeCustomerData();
  void InitializeDialogueData();
  void InitializeQuestChainsData();
  void InitializeNPCData();
  void InitializeCutsceneData();
  void InitializeMarketData();
  void InitializeNewsData();
  void InitializeUpgradesData();
};