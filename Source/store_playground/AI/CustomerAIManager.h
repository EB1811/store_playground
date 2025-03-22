// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/AI/CustomerDataStructs.h"
#include "store_playground/Npc/NpcDataStructs.h"
#include "store_playground/Store/Store.h"
#include "AIStructs.h"
#include "CustomerAIManager.generated.h"

USTRUCT()
struct FManagerParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  bool bSpawnCustomers;
  UPROPERTY(EditAnywhere)
  float CustomerSpawnInterval;
  UPROPERTY(EditAnywhere)
  int32 MaxCustomers;

  UPROPERTY(EditAnywhere)
  float UniqueNpcBaseSpawnChance;
  UPROPERTY(EditAnywhere)
  int32 UNpcMaxSpawnPerDay;

  UPROPERTY(EditAnywhere)
  float PickItemFrequency;
  UPROPERTY(EditAnywhere)
  int32 MaxCustomersPickingAtOnce;

  UPROPERTY(EditAnywhere)
  float PerformActionChance;
  UPROPERTY(EditAnywhere)
  TMap<ECustomerAction, float> ActionWeights;
};

USTRUCT()
struct FQuestInProgressData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  TArray<FName> ChainCompletedIDs;
  UPROPERTY(EditAnywhere)
  TArray<FName> ChoicesMade;
  UPROPERTY(EditAnywhere)
  TMap<FName, bool> NegotiationOutcomesMap;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ACustomerAIManager : public AInfo {
  GENERATED_BODY()

public:
  ACustomerAIManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Classes")
  TSubclassOf<class ACustomer> CustomerClass;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Data")
  const class AGlobalDataManager* GlobalDataManager;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Data")
  const class AMarket* Market;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Data")
  const class AMarketEconomy* MarketEconomy;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Data")
  const class AStore* Store;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Params")
  struct FManagerParams ManagerParams;

  // ? Seperate quests into another system?
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Tracking")
  TArray<FName> QuestsCompleted;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Tracking")
  TMap<FName, FQuestInProgressData> QuestInProgressMap;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Spawn Customers")
  float LastSpawnTime;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Spawn Customers")
  TArray<FName> RecentlySpawnedUniqueNpcIds;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Pick Item")
  float LastPickItemCheckTime;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Pick Item")
  FTimerHandle PickItemTimer;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Pick Item")
  TMap<FGuid, FGuid> PickingItemIdsMap;  // Item ID to Customer ID

  // ? Have separate arrays for each state?
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager | Customers Array")
  TArray<class ACustomer*> AllCustomers;

  void StartCustomerAI();
  void EndCustomerAI();

  void SpawnUniqueNpc();
  void SpawnCustomers();
  void PerformCustomerAILoop();

  void CustomerPerformAction(class UCustomerAIComponent* CustomerAI, class UInteractionComponent* Interaction);

  bool CustomerPickItem(class UCustomerAIComponent* CustomerAI,
                        std::function<bool(const FStockItem& StockItem)> FilterFunc = nullptr);
  bool CustomerStockCheck(class UCustomerAIComponent* CustomerAI,
                          std::function<bool(const FWantedItemType& ItemType)> FilterFunc = nullptr);
  void CustomerSellItem(class UCustomerAIComponent* CustomerAI, class UItemBase* Item = nullptr);
  void MakeCustomerNegotiable(class UCustomerAIComponent* CustomerAI, class UInteractionComponent* Interaction);

  void CompleteQuestChain(const FQuestChainData& QuestChainData,
                          TArray<FName> MadeChoiceIds = {},
                          bool bNegotiationSuccess = false);
};