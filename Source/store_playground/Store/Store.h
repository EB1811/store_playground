// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

  UPROPERTY(EditAnywhere)
  float Atmosphere;
  UPROPERTY(EditAnywhere)
  float Reputation;
};

USTRUCT()
struct FStoreLevelState {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  TMap<FGuid, FBuildableSaveState> BuildableSaveMap;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AStore : public AInfo {
  GENERATED_BODY()

public:
  AStore();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Store")
  class TSubclassOf<class AActor> BuildableClass;

  UPROPERTY(EditAnywhere, Category = "Store")
  FStoreLevelState StoreLevelState;

  UPROPERTY(EditAnywhere, Category = "Store")
  float Money;
  UPROPERTY(EditAnywhere, Category = "Store")
  FStoreStats StoreStats;

  UPROPERTY(EditAnywhere, Category = "Store")
  TArray<FStockItem> StoreStockItems;

  void SaveStoreLevelState();
  void LoadStoreLevelState();
  void InitStockDisplays();
};