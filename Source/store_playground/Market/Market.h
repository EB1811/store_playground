// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "store_playground/WorldObject/NPCStore.h"
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
  int32 RecentNpcSpawnedKeepTime;
  UPROPERTY(EditAnywhere)
  int32 RecentEconEventsKeepTime;
};

USTRUCT()
struct FMarketBehaviorParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float StoreMarkupMulti;
};

USTRUCT()
struct FMarketLevelState {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  TMap<FGuid, struct FNpcStoreSaveState> NpcStoreSaveMap;
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
  const class AQuestManager* QuestManager;

  UPROPERTY(EditAnywhere, Category = "Market")
  TObjectPtr<const class UDataTable> AllItemsTable;
  // ? Move class and casts to level manager, then pass as param to init functions?
  UPROPERTY(EditAnywhere, Category = "Market")
  class TSubclassOf<class ANPCStore> NPCStoreClass;
  UPROPERTY(EditAnywhere, Category = "Market")
  class TSubclassOf<class ANpcSpawnPoint> NpcSpawnPointClass;
  UPROPERTY(EditAnywhere, Category = "Market")
  class TSubclassOf<class ANpc> NpcClass;

  UPROPERTY(EditAnywhere, Category = "Market")
  FMarketParams MarketParams;
  UPROPERTY(EditAnywhere, Category = "Market")
  TArray<class UItemBase*> UnlockableItems;
  UPROPERTY(EditAnywhere, Category = "Market")
  TArray<class UItemBase*> EligibleItems;

  UPROPERTY(EditAnywhere, Category = "Market")
  FMarketBehaviorParams BehaviorParams;

  UPROPERTY(EditAnywhere, Category = "Market")
  class AMarketEconomy* MarketEconomy;

  UPROPERTY(EditAnywhere, Category = "Market")
  TMap<FName, int32> RecentlySpawnedUniqueNpcsMap;

  UPROPERTY(EditAnywhere, Category = "Market")
  TArray<FName> OccurredEconEvents;
  UPROPERTY(EditAnywhere, Category = "Market")
  TMap<FName, int32> RecentEconEventsMap;

  auto GetNewRandomItems(int32 Amount) const -> TArray<class UItemBase*>;
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

  auto ConsiderEconEvents() -> TArray<struct FEconEvent>;

  void TickDaysTimedVars();

  UPROPERTY(EditAnywhere, Category = "Market")
  FUpgradeable Upgradeable;
  void ChangeBehaviorParam(const TMap<FName, float>& ParamValues);
  void UnlockIDs(const FName DataName, const TArray<FName>& Ids);

  // ? Move level stuff to separate market level manager?
  UPROPERTY(EditAnywhere, Category = "Store")
  FMarketLevelState MarketLevelState;
  void SaveMarketLevelState();
  void LoadMarketLevelState();
  void ResetMarketLevelState();

  void InitNPCStores();
  void InitMarketNpcs();

  auto TrySpawnUniqueNpc(ANpcSpawnPoint* SpawnPoint, const FActorSpawnParameters& SpawnParams) -> bool;
};