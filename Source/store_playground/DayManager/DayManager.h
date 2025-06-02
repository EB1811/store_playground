// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "DayManager.generated.h"

// * Responsible for tracking the current day, starting new days, tracking times events, etc.

// TODO: Store debt owned day and amount.
USTRUCT()
struct FDayManagerParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  int32 WeekendDivisor;  // * Divisor for the weekend days, e.g., 5 = 1 weekend day every 5 days.

  // ? Or use a map for specific amounts?
  UPROPERTY(EditAnywhere)
  float BaseDebtAmount;
  UPROPERTY(EditAnywhere)
  float DebtIncreaseMulti;
  UPROPERTY(EditAnywhere)
  float DifficultyDebtIncreaseMulti;
  UPROPERTY(EditAnywhere)
  int32 DebtPaymentDayDivisor;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ADayManager : public AInfo {
  GENERATED_BODY()

public:
  ADayManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "DayManager")
  FDayManagerParams DayManagerParams;

  UPROPERTY(EditAnywhere, Category = "DayManager")
  class AAbilityManager* AbilityManager;  // * Tick active upgrades.
  UPROPERTY(EditAnywhere, Category = "DayManager")
  class ACustomerAIManager* CustomerAIManager;  // * TickDaysTimedVars
  UPROPERTY(EditAnywhere, Category = "DayManager")
  class AMarketEconomy* MarketEconomy;  // * Tick active price effects.
  UPROPERTY(EditAnywhere, Category = "DayManager")
  class AMarket* Market;  // * Start new day events.
  UPROPERTY(EditAnywhere, Category = "Store")
  class AStatisticsGen* StatisticsGen;  // * Store profit history, market history, etc.
  UPROPERTY(EditAnywhere, Category = "DayManager")
  class AMarketLevel* MarketLevel;  // * TickDaysTimedVars.
  UPROPERTY(EditAnywhere, Category = "DayManager")
  class ANewsGen* NewsGen;  // * Create new articles.

  UPROPERTY(EditAnywhere, Category = "DayManager")
  class AStore* Store;

  UPROPERTY(EditAnywhere, Category = "DayManager", SaveGame)
  int32 CurrentDay;  // * Incremental counter.
  UPROPERTY(EditAnywhere, Category = "DayManager", SaveGame)
  bool bIsWeekend;

  void StartNewDay();

  void ManageDebt();  // * Attempt to pay debt. If not enough money, game over.
};