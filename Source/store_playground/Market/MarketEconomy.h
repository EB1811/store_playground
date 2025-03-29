// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "MarketDataStructs.h"
#include "MarketEconomy.generated.h"

USTRUCT()
struct FEconomyParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float NeedsfulfilledPercent;  // Artificially set for 60% of the needs fulfilled at the start of the game.
  UPROPERTY(EditAnywhere)
  float SingleUnitPriceMulti;

  UPROPERTY(EditAnywhere)
  float BasePromotionChance;  // Promote to a higher wealth type.
  UPROPERTY(EditAnywhere)
  float BaseDemotionChance;  // Demote to a lower wealth type.
  UPROPERTY(EditAnywhere)
  float BaseCrossPromotionChance;  // Change to a different economy type.
};

USTRUCT()
struct FEconTypePrices {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  TMap<EItemWealthType, float> WealthTypePricesMap;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AMarketEconomy : public AInfo {
  GENERATED_BODY()

public:
  AMarketEconomy();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  // ? Move to data manager?
  // todo-low: Assert that customer data links to valid pop data.
  UPROPERTY(EditAnywhere, Category = "Economy")
  TObjectPtr<const class UDataTable> BasePopTypeToEconomyTable;
  UPROPERTY(EditAnywhere, Category = "Economy")
  TObjectPtr<const class UDataTable> CustomerPopDataTable;
  UPROPERTY(EditAnywhere, Category = "Economy")
  TObjectPtr<const class UDataTable> AllItemsTable;

  UPROPERTY(EditAnywhere, Category = "Economy")
  FEconomyParams EconomyParams;

  UPROPERTY(EditAnywhere, Category = "Economy")
  int32 TotalPopulation;
  UPROPERTY(EditAnywhere, Category = "Economy")
  float TotalWealth;
  UPROPERTY(EditAnywhere, Category = "Economy")
  float TotaBought;

  UPROPERTY(EditAnywhere, Category = "Economy")
  TArray<FCustomerPop> AllCustomerPops;
  UPROPERTY(EditAnywhere, Category = "Economy")
  TArray<FPopMoneySpendData> PopMoneySpendDataArray;

  UPROPERTY(EditAnywhere, Category = "Economy")
  TArray<FPriceEffect> ActivePriceEffects;

  UPROPERTY(EditAnywhere, Category = "Economy")
  TArray<FEconItem> EconItems;  // ? Also need fast lookups?
  UPROPERTY(EditAnywhere, Category = "Economy")
  TMap<EItemEconType, FEconTypePrices> EconTypePricesMap;

  void PerformEconomyTick();

  void InitializeEconomyData();
};

TArray<float> GetRandomSplit(int32 Buckets, float Money);