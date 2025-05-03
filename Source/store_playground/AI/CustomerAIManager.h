// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/AI/CustomerDataStructs.h"
#include "store_playground/Npc/NpcDataStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "store_playground/Store/Store.h"
#include "AIStructs.h"
#include "CustomerAIManager.generated.h"

USTRUCT()
struct FCustomerAIManagerParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  bool bSpawnCustomers;
  UPROPERTY(EditAnywhere)
  float CustomerSpawnInterval;

  UPROPERTY(EditAnywhere)
  float UniqueNpcBaseSpawnChance;
  UPROPERTY(EditAnywhere)
  int32 UNpcMaxSpawnPerDay;
  UPROPERTY(EditAnywhere)
  int32 RecentNpcSpawnedKeepTime;

  UPROPERTY(EditAnywhere)
  float PickItemFrequency;
  UPROPERTY(EditAnywhere)
  int32 MaxCustomersPickingAtOnce;
};

USTRUCT()
struct FCustomerAIBehaviorParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  int32 MaxCustomers;
  UPROPERTY(EditAnywhere, SaveGame)
  float CustomerSpawnChance;  // * Higher means more customers in the store at once.

  UPROPERTY(EditAnywhere, SaveGame)
  float PerformActionChance;
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<ECustomerAction, float> ActionWeights;

  UPROPERTY(EditAnywhere, SaveGame)
  float AcceptanceMinMulti;
  UPROPERTY(EditAnywhere, SaveGame)
  float AcceptanceMaxMulti;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ACustomerAIManager : public AInfo {
  GENERATED_BODY()

public:
  ACustomerAIManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  TSubclassOf<class ACustomer> CustomerClass;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  TSubclassOf<class ASpawnPoint> SpawnPointClass;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  const class AGlobalStaticDataManager* GlobalStaticDataManager;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  const class AMarket* Market;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  const class AMarketEconomy* MarketEconomy;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  const class AStore* Store;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  const class AQuestManager* QuestManager;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  struct FCustomerAIManagerParams ManagerParams;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager", SaveGame)
  struct FCustomerAIBehaviorParams BehaviorParams;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager", SaveGame)
  TMap<FName, int32> RecentlySpawnedUniqueNpcsMap;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  float LastSpawnTime;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  float LastPickItemCheckTime;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  FTimerHandle PickItemTimer;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  TMap<FGuid, FGuid> PickingItemIdsMap;  // Item ID to Customer ID

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  TArray<class ACustomer*> AllCustomers;

  void StartCustomerAI();
  void EndCustomerAI();

  void SpawnUniqueNpcs();
  void SpawnCustomers();
  void PerformCustomerAILoop();

  void MoveCustomerRandom(class UNavigationSystemV1* NavSystem, class ACustomer* Customer);
  void CustomerPerformAction(class ACustomer* Customer);

  bool CustomerPickItem(class UCustomerAIComponent* CustomerAI,
                        std::function<bool(const FStockItem& StockItem)> FilterFunc = nullptr);
  bool CustomerStockCheck(class UCustomerAIComponent* CustomerAI,
                          std::function<bool(const FWantedItemType& ItemType)> FilterFunc = nullptr);
  void CustomerSellItem(class UCustomerAIComponent* CustomerAI, class UItemBase* Item = nullptr);
  void MakeCustomerNegotiable(class ACustomer* Customer);

  void TickDaysTimedVars();

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  struct FUpgradeable Upgradeable;
  void ChangeBehaviorParam(const TMap<FName, float>& ParamValues);
  void UpgradeFunction(FName FunctionName, const TArray<FName>& Ids, const TMap<FName, float>& ParamValues);
};