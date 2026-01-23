// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/AI/CustomerDataStructs.h"
#include "store_playground/Npc/NpcDataStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "store_playground/Market/MarketDataStructs.h"
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
  float CustomerSpawnedDelay;  // Delay after spawning customers before doing actions (just moving around).
  UPROPERTY(EditAnywhere)
  int32 MaxSpawnCustomersInOneGo;  // Avoid freezing the game by spawning too many customers at once.

  UPROPERTY(EditAnywhere)
  float UniqueNpcBaseSpawnChance;
  UPROPERTY(EditAnywhere)
  int32 UNpcMaxSpawnPerDay;
  UPROPERTY(EditAnywhere)
  int32 RecentNpcSpawnedKeepTime;

  UPROPERTY(EditAnywhere)
  float PopMoneyToCustomerMoneyMulti;

  UPROPERTY(EditAnywhere)
  float PickItemFrequency;

  UPROPERTY(EditAnywhere)
  float BaseAcceptMin;
  UPROPERTY(EditAnywhere)
  float BaseAcceptMax;
  UPROPERTY(EditAnywhere)
  float BaseAcceptFalloffMulti;
};

USTRUCT()
struct FCustomerAIBehaviorParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  float StockDisplayToCustomerRatio;  // Number of additional customers to spawn per stock display count.
  UPROPERTY(EditAnywhere, SaveGame)
  int32 MaxCustomers;
  UPROPERTY(EditAnywhere, SaveGame)
  float WeekendCustomerMulti;
  UPROPERTY(EditAnywhere, SaveGame)
  float CustomerSpawnChance;  // * Higher means more customers in the store at once.
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<EPopType, float> PopTypeMultis;
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<EPopWealthType, float> PopWealthTypeMultis;

  UPROPERTY(EditAnywhere, SaveGame)
  float PerformActionChance;
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<ECustomerAction, float> ActionWeights;

  UPROPERTY(EditAnywhere, SaveGame)
  float AvailableMoneyMulti;  // * Multiplier for the available money of the customer.
  UPROPERTY(EditAnywhere, SaveGame)
  float WeekendAvailableMoneyMulti;

  UPROPERTY(EditAnywhere, SaveGame)
  float AcceptanceMinMulti;
  UPROPERTY(EditAnywhere, SaveGame)
  float AcceptanceMaxMulti;
  UPROPERTY(EditAnywhere, SaveGame)
  float AcceptanceFalloffMulti;
  UPROPERTY(EditAnywhere, SaveGame)
  float PostHagglingAcceptanceMulti;  // * If npc counteroffers, the acceptance drops.
  UPROPERTY(EditAnywhere, SaveGame)
  int32 InitHagglingCount;  // * Decreases after each attempt.
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<EPopType, float> PopTypeAcceptMultis;  // * Multiplier for acceptance based on pop type.
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<EPopWealthType, float> PopWealthTypeAcceptMultis;  // * Multiplier for acceptance based on pop wealth type.
  UPROPERTY(EditAnywhere, SaveGame)
  float AnyPriceAcceptanceChance;  // * An additional flat check to accept any price.

  UPROPERTY(EditAnywhere, SaveGame)
  float CustomerWaitingTime;  // * Time the customer waits before leaving.

  UPROPERTY(EditAnywhere, SaveGame)
  bool bSpawnBasedOnItemsInStore;  // * Upgrade, if true, spawn customers based on items displayed.
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ACustomerAIManager : public AInfo {
  GENERATED_BODY()

public:
  ACustomerAIManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  TSubclassOf<class ACustomerPC> CustomerClass;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  TSubclassOf<class ASpawnPoint> SpawnPointClass;

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  const class APlayerZDCharacter* PlayerCharacter;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  const class AGlobalStaticDataManager* GlobalStaticDataManager;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  const class ADayManager* DayManager;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  const class AAbilityManager* AbilityManager;
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
  TArray<class ACustomerPC*> AllCustomers;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  TArray<class ACustomerPC*> ExitingCustomers;
  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  TArray<class ACustomerPC*> RequestingCustomers;

  void StartCustomerAI();
  void EndCustomerAI();

  void SpawnUniqueNpcs();
  void SpawnCustomers();
  void PerformCustomerAILoop();

  void MoveCustomerRandom(class UNavigationSystemV1* NavSystem, class ACustomerPC* Customer);
  void MoveCustomerToExit(class UNavigationSystemV1* NavSystem, class ACustomerPC* Customer, FVector ExitLocation);

  void CustomerPerformAction(class ACustomerPC* Customer, TMap<ECustomerAction, float> ActionWeights);

  auto CustomerPickItem(class UCustomerAIComponent* CustomerAI,
                        std::function<bool(const FStockItem& StockItem)> FilterFunc = nullptr) -> bool;
  auto CustomerStockCheck(class UCustomerAIComponent* CustomerAI,
                          std::function<bool(const FWantedItemType& ItemType)> FilterFunc = nullptr) -> bool;
  auto CustomerSellItem(class UCustomerAIComponent* CustomerAI, class UItemBase* Item = nullptr) -> bool;
  void MakeCustomerNegotiable(class ACustomerPC* Customer);

  auto ConsiderStockCheck(class UCustomerAIComponent* CustomerAI, const class UItemBase* Item) const -> FOfferResponse;
  auto GetPriceAcceptanceChance(class UCustomerAIComponent* CustomerAI,
                                float MarketPrice,
                                float LastOfferedPrice,
                                float PlayerOfferedPrice) const -> float;
  auto ConsiderOffer(class UCustomerAIComponent* CustomerAI,
                     const class UItemBase* Item,
                     float LastOfferedPrice,
                     float PlayerOfferedPrice) const -> FOfferResponse;

  void StockUpdated();

  void TickDaysTimedVars();

  UPROPERTY(EditAnywhere, Category = "CustomerAIManager")
  struct FUpgradeable Upgradeable;
  void ChangeBehaviorParam(const TMap<FName, float>& ParamValues);
  void UpgradeFunction(FName FunctionName, const TArray<FName>& Ids, const TMap<FName, float>& ParamValues);
  void ChangeActionWeights(const TMap<FName, float>& ParamValues);
  void ChangePopTypeMultis(const TMap<FName, float>& ParamValues);
  void ChangePopWealthTypeMultis(const TMap<FName, float>& ParamValues);
  void ChangePopTypeAcceptMultis(const TMap<FName, float>& ParamValues);
  void ChangePopWealthTypeAcceptMultis(const TMap<FName, float>& ParamValues);
};