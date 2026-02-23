// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "store_playground/WorldObject/Buildable.h"
#include "GameFramework/Info.h"
#include "Store.generated.h"

// ? Probably should be separated into a store level class.

USTRUCT()
struct FStockItem {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ItemId;  // * To identify items when outside the store.

  UPROPERTY(EditAnywhere)
  class UStockDisplayComponent* StockDisplayComponent;
  UPROPERTY(EditAnywhere)
  class UItemBase* Item;
  UPROPERTY(EditAnywhere)
  class UInventoryComponent* BelongingStockInventoryC;
};
USTRUCT()
struct FStoreStats {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  float Atmosphere;
  UPROPERTY(EditAnywhere, SaveGame)
  float Reputation;
};

USTRUCT()
struct FStoreLevelState {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  TMap<FGuid, FActorSavaState> ActorSaveMap;
  UPROPERTY(EditAnywhere)
  TMap<FGuid, FComponentSaveState> ComponentSaveMap;
  UPROPERTY(EditAnywhere)
  TArray<FObjectSaveState> ObjectSaveStates;
};

USTRUCT()
struct FStoreBehaviorParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  float BuildCostMulti;
  UPROPERTY(EditAnywhere, SaveGame)
  double NegativeMoneyLimit;  // * Positive value indicating how much below zero the player can go.
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AStore : public AInfo {
  GENERATED_BODY()

public:
  AStore();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Store")
  const class ASaveManager* SaveManager;
  UPROPERTY(EditAnywhere, Category = "Store")
  const class AStorePhaseManager* StorePhaseManager;

  UPROPERTY(EditAnywhere, Category = "Store")
  class TSubclassOf<class AActor> BuildableClass;

  UPROPERTY(EditAnywhere, Category = "Store")
  class ACustomerAIManager* CustomerAIManager;
  UPROPERTY(EditAnywhere, Category = "Store")
  class AStatisticsGen* StatisticsGen;
  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class AStorePhaseLightingManager* StorePhaseLightingManager;
  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class AAmbientSoundManager* AmbientSoundManager;

  UPROPERTY(EditAnywhere, Category = "Store", SaveGame)
  struct FStoreBehaviorParams BehaviorParams;

  UPROPERTY(EditAnywhere, Category = "Store", SaveGame)
  double Money;
  UPROPERTY(EditAnywhere, Category = "Store", SaveGame)
  FStoreStats StoreStats;

  UPROPERTY(EditAnywhere, Category = "Store")
  int32 StockDisplayCount;  // * To calculate customer spawning, etc.
  UPROPERTY(EditAnywhere, Category = "Store")
  TArray<FStockItem> StoreStockItems;

  auto GetBuildablePrice(ABuildable* Buildable) -> float;
  auto BuildStockDisplay(ABuildable* Buildable) -> bool;
  auto BuildDecoration(ABuildable* Buildable) -> bool;

  auto GetAvailableMoney() const -> double;  // * Includes all modifiers, not for display.
  auto TrySpendMoney(float Amount) -> bool;

  void NegStockItemSold(const UItemBase* Item);
  void NegItemSold(const UItemBase* Item, float SingleUnitPrice, int32 Quantity = 1);

  void ItemBought(UItemBase* Item, float SingleUnitPrice, int32 Quantity = 1);

  void MoneyGained(float Amount);
  void MoneySpent(float Amount);

  void EnterLevel();
  void SetupStoreEnvironment();

  UPROPERTY(EditAnywhere, Category = "Store")
  FStoreLevelState StoreLevelState;
  void SaveStoreLevelState();
  void LoadStoreLevelState();
  void InitStockDisplays();

  UPROPERTY(EditAnywhere, Category = "Store")
  struct FUpgradeable Upgradeable;
  void ChangeBehaviorParam(const TMap<FName, float>& ParamValues);
};