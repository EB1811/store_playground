// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "MarketDataStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "MarketEconomy.generated.h"

USTRUCT()
struct FEconomyParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  bool bRunSimulation;

  UPROPERTY(EditAnywhere)
  float NeedsfulfilledPercent;  // Artificially set for 60% of the needs fulfilled at the start of the game.
  UPROPERTY(EditAnywhere)
  float SingleUnitPriceMulti;

  UPROPERTY(EditAnywhere)
  float EssentialNeedsPercent;  // Percentage of needs that must be at least fulfilled to avoid demotion.
  UPROPERTY(EditAnywhere)
  float LuxaryNeedsPercent;  // Percentage of needs that must be at least fulfilled to consider promotion.
};

USTRUCT()
struct FEconomyBehaviorParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  float PromotionChance;  // Promote to a higher wealth type.
  UPROPERTY(EditAnywhere, SaveGame)
  float DemotionChance;  // Demote to a lower wealth type.
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<EPopWealthType, float> CrossPromotionChanceMap;  // Cross promote to a different pop type. For each wealth type.
  UPROPERTY(EditAnywhere, SaveGame)
  float CrossPromotionChanceMulti;  // Multiplier for cross promotion chance.

  UPROPERTY(EditAnywhere, SaveGame)
  int32 MaxPopChangeAtOnce;  // Max population change per tick.
};

USTRUCT()
struct FEconTypePrices {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
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
  class AStatisticsGen* StatisticsGen;

  UPROPERTY(EditAnywhere, Category = "Economy")
  int32 TotalPopulation;
  UPROPERTY(EditAnywhere, Category = "Economy")
  float TotalWealth;
  UPROPERTY(EditAnywhere, Category = "Economy")
  float TotaBought;

  UPROPERTY(EditAnywhere, Category = "Economy", SaveGame)
  FEconomyBehaviorParams BehaviorParams;

  UPROPERTY(EditAnywhere, Category = "Economy")
  TArray<FCustomerPop> CustomerPops;
  UPROPERTY(EditAnywhere, Category = "Economy", SaveGame)
  TArray<FPopEconData> PopEconDataArray;

  // TODO: Add price effect ramp up.
  UPROPERTY(EditAnywhere, Category = "Economy", SaveGame)
  TArray<FPriceEffect> ActivePriceEffects;
  UPROPERTY(EditAnywhere, Category = "Economy", SaveGame)
  TArray<FPopEffect> ActivePopEffects;

  UPROPERTY(EditAnywhere, Category = "Economy", SaveGame)
  TArray<FEconItem> EconItems;  // ? Also need fast lookups?
  UPROPERTY(EditAnywhere, Category = "Economy", SaveGame)
  TMap<EItemEconType, FEconTypePrices> EconTypePricesMap;

  auto GetMarketPrice(const FName ItemId) const -> float;

  void PerformEconomyTick();

  auto GetPopWeightingMulti(const FCustomerPop& Pop) const -> float;
  auto GetPopWeightingMulti(const FPopEconData& PopEconData) const -> float;

  void AddPriceEffect(const FPriceEffect& PriceEffect);

  void TickDaysActivePriceEffects();

  void InitializeEconomyData();

  UPROPERTY(EditAnywhere, Category = "Economy")
  FUpgradeable Upgradeable;
  void SetupUpgradeable();
};