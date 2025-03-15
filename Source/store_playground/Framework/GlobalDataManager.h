// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Npc/NpcDataStructs.h"
#include "store_playground/AI/CustomerDataStructs.h"
#include "GlobalDataManager.generated.h"

// * Global data store to hold data used by random generation systems.
// * Data comes from the data tables.
// * Actor due to the need to initialize the data and possible tick to update it / check for changes.

UENUM()
enum class EReqFilterOperand : uint8 {
  Time UMETA(DisplayName = "Time"),  // ? Review if needed.
  Money UMETA(DisplayName = "Money"),
  Inventory UMETA(DisplayName = "Inventory"),                        // * Using item ids.
  CompletedQuests UMETA(DisplayName = "Completed Quests"),           // * Using quest ids.
  MadeDialogueChoices UMETA(DisplayName = "Made Dialogue Choices"),  // * Using dialogue chain ids.
};
ENUM_RANGE_BY_COUNT(EReqFilterOperand, 5);
USTRUCT()
struct FFilterGameData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float PlayerMoney;
  // UPROPERTY(EditAnywhere)
  // TArray<FName> InventoryItemIDs;
  UPROPERTY(EditAnywhere)
  TArray<FName> CompletedQuestIDs;
};

bool EvaluateRequirementsFilter(const FName& RequirementsFilter, FFilterGameData GameData);

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AGlobalDataManager : public AInfo {
  GENERATED_BODY()

public:
  AGlobalDataManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

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
  TObjectPtr<const class UDataTable> FQuestChainDataDataTable;

  UPROPERTY(EditAnywhere, Category = "Store")
  TArray<struct FGenericCustomerData> GenericCustomersArray;
  UPROPERTY(EditAnywhere, Category = "Store")
  TArray<struct FWantedItemType> WantedItemTypesArray;

  UPROPERTY(EditAnywhere, Category = "Store")
  TMap<FName, FDialoguesArray> UniqueNpcDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Store")
  TMap<FName, FDialoguesArray> QuestDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Store")
  TArray<struct FDialogueData> CustomerDialogues;
  UPROPERTY(EditAnywhere, Category = "Store")
  TMap<ENegotiationDialogueType, FDialoguesArray> FriendlyDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Store")
  TMap<ENegotiationDialogueType, FDialoguesArray> NeutralDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Store")
  TMap<ENegotiationDialogueType, FDialoguesArray> HostileDialoguesMap;

  UPROPERTY(EditAnywhere, Category = "Store")
  TArray<struct FUniqueNpcData> UniqueNpcArray;
  UPROPERTY(EditAnywhere, Category = "Store")
  TArray<struct FQuestChainData> QuestChainsArray;  // ? Change to TMap?

  void InitializeCustomerData();
  void InitializeDialogueData();
  void InitializeQuestChainsData();
  void InitializeNPCData();

  TArray<struct FUniqueNpcData> GetEligibleNpcs(FFilterGameData GameData) const;
  TArray<struct FQuestChainData> GetEligibleQuestChains(const TArray<FName>& QuestIDs,
                                                        FFilterGameData GameData,
                                                        TMap<FName, FName> QuestsInProgressMap) const;
  TArray<struct FDialogueData> GetQuestDialogue(const FQuestChainData& QuestChain) const;

  TArray<struct FDialogueData> GetRandomNpcDialogue(const TArray<FName>& DialogueChainIDs) const;
  TArray<struct FDialogueData> GetRandomCustomerDialogue() const;
  TMap<ENegotiationDialogueType, FDialoguesArray> GetRandomNegDialogueMap(
      ECustomerAttitude Attitude = ECustomerAttitude::Neutral) const;
};