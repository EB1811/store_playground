// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/WorldObject/Buildable.h"
#include "GameFramework/Info.h"
#include "Store.generated.h"

USTRUCT()
struct FStockItem {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  struct FDisplayStats DisplayStats;
  UPROPERTY(EditAnywhere)
  class UItemBase* Item;  // ? Change to item id?
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

  UPROPERTY()
  TMap<FGuid, FActorSavaState> ActorSaveMap;
  UPROPERTY()
  TMap<FGuid, FComponentSaveState> ComponentSaveMap;
  UPROPERTY()
  TArray<FObjectSaveState> ObjectSaveStates;
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
  class TSubclassOf<class AActor> BuildableClass;

  UPROPERTY(EditAnywhere, Category = "Store")
  class AStatisticsGen* StatisticsGen;

  UPROPERTY(EditAnywhere, Category = "Store", SaveGame)
  float Money;
  UPROPERTY(EditAnywhere, Category = "Store", SaveGame)
  FStoreStats StoreStats;

  UPROPERTY(EditAnywhere, Category = "Store")
  TArray<FStockItem> StoreStockItems;

  void ItemBought(UItemBase* Item, float Price, int32 Quantity = 1);
  void ItemSold(const UItemBase* Item, float Price, int32 Quantity = 1);
  void MoneyGained(float Amount);
  void MoneySpent(float Amount);

  UPROPERTY(EditAnywhere, Category = "Store")
  FStoreLevelState StoreLevelState;
  void SaveStoreLevelState();
  void LoadStoreLevelState();
  void InitStockDisplays();
};