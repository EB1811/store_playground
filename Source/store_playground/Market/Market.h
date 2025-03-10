// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Market.generated.h"

USTRUCT()
struct FMarketParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  int32 MinPrice;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AMarket : public AInfo {
  GENERATED_BODY()

public:
  AMarket();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Market")
  TObjectPtr<const class UDataTable> NpcStoreDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Market")
  TObjectPtr<const class UDataTable> AllItemsTable;
  // * One store for now.
  UPROPERTY(EditAnywhere, Category = "Market")
  class TSubclassOf<class AActor> NPCStoreClass;

  UPROPERTY(EditAnywhere, Category = "Market")
  TArray<struct FDialogueData> NpcStoreDialogues;

  UPROPERTY(EditAnywhere, Category = "Market")
  TArray<class UItemBase*> AllItems;
  UPROPERTY(EditAnywhere, Category = "Market")
  TMap<FName, float> MarketPrices;

  void InitializeNPCStores();

  TArray<int32> GetRandomDialogueIndexes();
  TArray<class UItemBase*> GetNewRandomItems(int32 Amount);
};

void BuyItem(class AMarketEconomy* MarketEconomy,
             class UInventoryComponent* NPCStoreInventory,
             class UInventoryComponent* PlayerInventory,
             class AStore* PLayerStore,
             class UItemBase* Item,
             int32 Quantity = 1);
void SellItem(class AMarketEconomy* MarketEconomy,
              class UInventoryComponent* NPCStoreInventory,
              class UInventoryComponent* PlayerInventory,
              class AStore* PlayerStore,
              class UItemBase* Item,
              int32 Quantity = 1);