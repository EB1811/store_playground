// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Store.generated.h"

USTRUCT()
struct FStockItem {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  class UInventoryComponent* BelongingStockInventoryC;
  UPROPERTY(EditAnywhere)
  class UItemBase* Item;
};

USTRUCT()
struct FStoreStats {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float Atmosphere;
  UPROPERTY(EditAnywhere)
  float Reputation;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AStore : public AInfo {
  GENERATED_BODY()

public:
  AStore();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Store Data")
  float Money;
  UPROPERTY(EditAnywhere, Category = "Store Data")
  FStoreStats StoreStats;

  UPROPERTY(EditAnywhere, Category = "Store Stock")
  TArray<FStockItem> StoreStockItems;

  // ? Put state in game manager?
  // UPROPERTY(EditAnywhere, Category = "Store")
  // state

  UPROPERTY(EditAnywhere, Category = "TESTING")
  class TSubclassOf<class AActor> StockDisplayClass;
  void InitStockDisplays();
};