// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/StatisticsGen/StatisticsStructs.h"
#include "StatisticsGen.generated.h"

// * Store profit history, market history, etc.
// Using arrays since queues are not supported for reflection.

USTRUCT()
struct FStatisticsGenParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  int32 MaxHistoryCount;  // * In days.
};

USTRUCT()
struct FStoreStatistics {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  int32 CurrentHistoryCount;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<float> ProfitHistory;
  UPROPERTY(EditAnywhere, SaveGame)
  float TotalProfitToDate;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<float> RevenueHistory;
  UPROPERTY(EditAnywhere, SaveGame)
  float TotalRevenueToDate;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<float> ExpensesHistory;
  UPROPERTY(EditAnywhere, SaveGame)
  float TotalExpensesToDate;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<float> NetWorthHistory;
};
USTRUCT()
struct FItemStatistics {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  FName ItemId;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<float> PriceHistory;
};
USTRUCT()
struct FPopStatistics {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<float> PopulationHistory;
  UPROPERTY(EditAnywhere, SaveGame)
  float TodaysPopulationChange;
  UPROPERTY(EditAnywhere, SaveGame)
  TArray<float> GoodsBoughtPerCapitaHistory;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AStatisticsGen : public AInfo {
  GENERATED_BODY()

public:
  AStatisticsGen() { PrimaryActorTick.bCanEverTick = false; }

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  const class UInventoryComponent* PlayerInventoryC;
  UPROPERTY(EditAnywhere)
  const class AStore* Store;
  UPROPERTY(EditAnywhere)
  const class AMarketEconomy* MarketEconomy;

  UPROPERTY(EditAnywhere)
  FStatisticsGenParams StatisticsGenParams;

  UPROPERTY(EditAnywhere, SaveGame)
  FStoreStatistics StoreStatistics;
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<FName, FItemStatistics> ItemStatisticsMap;
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<FName, FPopStatistics> PopStatisticsMap;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FItemDeal> TodaysItemDeals;  // * Items sold at negotiation or market.
  UPROPERTY(EditAnywhere, SaveGame)
  FStoreMoneyActivity TodaysStoreMoneyActivity;  // * Money spent, money made.

  void ItemDeal(const FItemDeal ItemDeal);
  void StoreMoneyGained(float Amount);
  void StoreMoneySpent(float Amount);

  auto CalcTodaysStoreProfit() const -> float;
  auto CalcNetWorth() const -> float;

  void ItemPriceChange(const FName ItemId, const float NewPrice);
  void PopChange(const FName PopId, float NewPopulation, float NewGoodsBoughtPerCapita);

  void CalcDayStatistics();  // * Call at the end of the day and reset the daily statistics.
};