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
  class UItemBase* Item;  // ? Change to item id?
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

  UPROPERTY(EditAnywhere, Category = "Store")
  class TSubclassOf<class AActor> BuildableClass;

  UPROPERTY(EditAnywhere, Category = "Store")
  float Money;
  UPROPERTY(EditAnywhere, Category = "Store")
  FStoreStats StoreStats;

  // TODO: Initialize stock displays.
  UPROPERTY(EditAnywhere, Category = "Store")
  TArray<FStockItem> StoreStockItems;

  void InitStockDisplays();
};