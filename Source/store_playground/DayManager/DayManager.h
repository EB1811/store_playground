// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "DayManager.generated.h"

// * Responsible for tracking the current day, starting new days, tracking times events, etc.

// USTRUCT()
// struct FDayManagerParams {
//   GENERATED_BODY()
// };

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ADayManager : public AInfo {
  GENERATED_BODY()

public:
  ADayManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "DayManager")
  class ACustomerAIManager* CustomerAIManager;  // * TickDaysTimedVars
  UPROPERTY(EditAnywhere, Category = "DayManager")
  class AMarketEconomy* MarketEconomy;  // * Tick active price effects.
  UPROPERTY(EditAnywhere, Category = "DayManager")
  class AMarket* Market;  // * Start new day events.
  UPROPERTY(EditAnywhere, Category = "DayManager")
  class ANewsGen* NewsGen;  // * Create new articles.

  UPROPERTY(EditAnywhere, Category = "DayManager")
  int32 CurrentDay;  // * Incremental counter.

  void StartNewDay();
};