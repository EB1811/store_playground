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
  int32 RecentNpcSpawnedKeepTime;
  UPROPERTY(EditAnywhere)
  int32 RecentEconEventsKeepTime;
};

USTRUCT()
struct FMarketBehaviorParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  float StoreMarkupMulti;
};

USTRUCT()
struct FNpcStoreSaveState {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FGuid Id;

  UPROPERTY(EditAnywhere)
  TArray<struct FDialogueData> DialogueArray;
  UPROPERTY(EditAnywhere)
  TArray<TObjectPtr<class UItemBase>> ItemsArray;
  UPROPERTY(EditAnywhere)
  FNpcStoreType NpcStoreType;
};
// * Simplified save state since its not saved to disc.
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
  // TODO: Move to data manager and store ids.
  UPROPERTY(EditAnywhere, Category = "Market")
  TMap<FName, class UItemBase*> AllItemsMap;
  UPROPERTY(EditAnywhere, Category = "Market", SaveGame)
  TArray<FName> EligibleItemIds;

  UPROPERTY(EditAnywhere, Category = "Market", SaveGame)
  FMarketBehaviorParams BehaviorParams;

  UPROPERTY(EditAnywhere, Category = "Market")
  class AMarketEconomy* MarketEconomy;

  UPROPERTY(EditAnywhere, Category = "Market", SaveGame)
  TMap<FName, int32> RecentlySpawnedUniqueNpcsMap;

  UPROPERTY(EditAnywhere, Category = "Market", SaveGame)
  TArray<FName> OccurredEconEvents;
  UPROPERTY(EditAnywhere, Category = "Market", SaveGame)
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
  UPROPERTY(EditAnywhere, Category = "Market")
  FMarketLevelState MarketLevelState;
  void SaveMarketLevelState();
  void LoadMarketLevelState();
  void ResetMarketLevelState();

  void InitNPCStores();
  void InitMarketNpcs();
  auto TrySpawnUniqueNpc(ANpcSpawnPoint* SpawnPoint, const FActorSpawnParameters& SpawnParams) -> bool;
};