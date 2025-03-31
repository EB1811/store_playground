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
#include "GlobalDataManager.generated.h"

// * Global data store to hold data used by random generation systems.
// * Data comes from the data tables.
// * Actor due to the need to initialize the data and possible tick to update it / check for changes.

UENUM()
enum class EReqFilterOperand : uint8 {
  Time UMETA(DisplayName = "Time"),  // ? Review if needed.
  Money UMETA(DisplayName = "Money"),
  Inventory UMETA(DisplayName = "Inventory"),                      // * Using item ids.
  QuestsCompleted UMETA(DisplayName = "QuestsCompleted"),          // * Using quest ids.
  MadeDialogueChoices UMETA(DisplayName = "MadeDialogueChoices"),  // * Using dialogue chain ids.
  RecentEconEvents UMETA(DisplayName = "RecentEconEvents"),        // * Using econ event ids.
};
ENUM_RANGE_BY_COUNT(EReqFilterOperand, 5);

bool EvaluateRequirementsFilter(const FName& RequirementsFilter, const TMap<EReqFilterOperand, std::any>& GameDataMap);

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AGlobalDataManager : public AInfo {
  GENERATED_BODY()

public:
  AGlobalDataManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  // * To retrieve game's state for evaluating requirements.
  UPROPERTY(EditAnywhere, Category = "PlayerData")
  const class APlayerZDCharacter* PlayerCharacter;
  const TMap<EReqFilterOperand, std::any> GetGameDataMap() const;

  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> GenericCustomersDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> WantedItemTypesDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> UniqueNpcDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> QuestDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> CustomerDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  struct FDataTableCategoryHandle FriendlyNegDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  struct FDataTableCategoryHandle NeutralNegDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  struct FDataTableCategoryHandle HostileNegDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> UniqueNpcDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> QuestChainDataDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> NpcStoreTypesDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> NpcStoreDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> PriceEffectsDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> EconEventsDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> ArticlesDataTable;

  UPROPERTY(EditAnywhere, Category = "GenericCustomer")
  TArray<struct FGenericCustomerData> GenericCustomersArray;
  UPROPERTY(EditAnywhere, Category = "GenericCustomer")
  TArray<struct FWantedItemType> WantedItemTypesArray;

  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TMap<FName, FDialoguesArray> UniqueNpcDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TMap<FName, FDialoguesArray> QuestDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TArray<struct FDialogueData> CustomerDialogues;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TMap<ENegotiationDialogueType, FDialoguesArray> FriendlyDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TMap<ENegotiationDialogueType, FDialoguesArray> NeutralDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Dialogues")
  TMap<ENegotiationDialogueType, FDialoguesArray> HostileDialoguesMap;

  UPROPERTY(EditAnywhere, Category = "UniqueNpc")
  TArray<struct FUniqueNpcData> UniqueNpcArray;
  UPROPERTY(EditAnywhere, Category = "UniqueNpc")
  TArray<struct FQuestChainData> QuestChainsArray;  // ? Change to TMap?

  UPROPERTY(EditAnywhere, Category = "NpcStore")
  TArray<struct FNpcStoreType> NpcStoreTypesArray;
  UPROPERTY(EditAnywhere, Category = "NpcStore")
  TArray<struct FDialogueData> NpcStoreDialogues;

  UPROPERTY(EditAnywhere, Category = "Market")
  TArray<struct FPriceEffect> PriceEffectsArray;
  UPROPERTY(EditAnywhere, Category = "Market")
  TArray<struct FEconEvent> EconEventsArray;
  UPROPERTY(EditAnywhere, Category = "News")
  TArray<struct FArticle> ArticlesArray;

  void InitializeCustomerData();
  void InitializeDialogueData();
  void InitializeQuestChainsData();
  void InitializeNPCData();
  void InitializeNpcStoreData();
  void InitializeMarketData();
  void InitializeNewsData();

  TArray<struct FUniqueNpcData> GetEligibleNpcs() const;
  TArray<struct FQuestChainData> GetEligibleQuestChains(const TArray<FName>& QuestIDs,
                                                        TArray<FName> CompletedQuestIDs,
                                                        TMap<FName, FName> PrevChainCompletedMap) const;
  TArray<struct FDialogueData> GetQuestDialogue(const FName& DialogueChainID) const;

  TArray<struct FDialogueData> GetRandomNpcDialogue(const TArray<FName>& DialogueChainIDs) const;
  TArray<struct FDialogueData> GetRandomCustomerDialogue() const;
  TArray<struct FDialogueData> GetRandomNpcStoreDialogue() const;
  TMap<ENegotiationDialogueType, FDialoguesArray> GetRandomNegDialogueMap(
      ECustomerAttitude Attitude = ECustomerAttitude::Neutral) const;

  TArray<struct FEconEvent> GetEligibleEconEvents(const TArray<FName>& OccurredEconEvents) const;
  TArray<struct FPriceEffect> GetPriceEffects(const TArray<FName>& PriceEffectIDs) const;

  TArray<struct FArticle> GetEligibleArticles(const TArray<FName>& PublishedArticles) const;
  FArticle GetArticle(const FName& ArticleID) const;
};