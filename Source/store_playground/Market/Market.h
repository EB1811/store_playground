// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "Market.generated.h"

USTRUCT()
struct FMarketParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float UniqueNpcBaseSpawnChance;
  UPROPERTY(EditAnywhere)
  float BaseEconEventStartChance;  // * Chance to start for each event.

  UPROPERTY(EditAnywhere)
  int32 RecentEconEventsKeepTime;
};

USTRUCT()
struct FMarketBehaviorParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  float StoreMarkupMulti;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AMarket : public AInfo {
  GENERATED_BODY()

public:
  AMarket();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Market")
  const class AGlobalDataManager* GlobalDataManager;
  UPROPERTY(EditAnywhere, Category = "Market")
  const class AGlobalStaticDataManager* GlobalStaticDataManager;
  UPROPERTY(EditAnywhere, Category = "Market")
  const class AQuestManager* QuestManager;

  UPROPERTY(EditAnywhere, Category = "Market")
  TObjectPtr<const class UDataTable> AllItemsTable;

  UPROPERTY(EditAnywhere, Category = "Market")
  FMarketParams MarketParams;

  // ? Move to data manager.
  UPROPERTY(EditAnywhere, Category = "Market")
  TMap<FName, class UItemBase*> AllItemsMap;
  UPROPERTY(EditAnywhere, Category = "Market", SaveGame)
  TArray<FName> EligibleItemIds;

  UPROPERTY(EditAnywhere, Category = "Market", SaveGame)
  FMarketBehaviorParams BehaviorParams;

  UPROPERTY(EditAnywhere, Category = "Market")
  class AMarketEconomy* MarketEconomy;

  UPROPERTY(EditAnywhere, Category = "Market", SaveGame)
  TArray<FName> GuaranteedEconEventIDs;  // * Guaranteed econ events to occur the next available time.
  UPROPERTY(EditAnywhere, Category = "Market", SaveGame)
  TArray<FName> OccurredEconEvents;
  UPROPERTY(EditAnywhere, Category = "Market", SaveGame)
  TMap<FName, int32> RecentEconEventsMap;
  UPROPERTY(EditAnywhere, Category = "Market", SaveGame)
  TArray<struct FEconEvent> TodaysEconEvents;  // * For world to react.

  auto GetNewRandomItems(int32 Amount,
                         TArray<EItemType> ItemTypes = {},
                         TArray<EItemWealthType> ItemWealthTypes = {},
                         TArray<EItemEconType> ItemEconType = {}) const -> TArray<class UItemBase*>;
  auto GetRandomItem(const TArray<FName> ItemIds) const -> class UItemBase*;

  auto BuyItem(class UNpcStoreComponent* NpcStoreC,
               class UInventoryComponent* NPCStoreInventory,
               class UInventoryComponent* PlayerInventory,
               class AStore* PlayerStore,
               class UItemBase* Item,
               int32 Quantity = 1) const -> bool;
  auto SellItem(class UNpcStoreComponent* NpcStoreC,
                class UInventoryComponent* NPCStoreInventory,
                class UInventoryComponent* PlayerInventory,
                class AStore* PlayerStore,
                class UItemBase* Item,
                int32 Quantity = 1) const -> bool;

  void AddGuaranteedEconEvents(const TArray<FName>& EconEventIDs);
  auto ConsiderEconEvents() -> TArray<struct FEconEvent>;

  void TickDaysTimedVars();

  UPROPERTY(EditAnywhere, Category = "Market")
  FUpgradeable Upgradeable;
  void ChangeBehaviorParam(const TMap<FName, float>& ParamValues);
  void UnlockIDs(const FName DataName, const TArray<FName>& Ids);
};