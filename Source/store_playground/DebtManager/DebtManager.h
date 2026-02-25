// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Framework/SettingsStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "DebtManager.generated.h"

USTRUCT()
struct FDebtManagerParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float BaseDebtInterest;
  UPROPERTY(EditAnywhere)
  float BaseDebtInterestPerDay;
  UPROPERTY(EditAnywhere)
  float BaseDebtLimit;
};
USTRUCT()
struct FDebtManagerBehaviorParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  float DebtInterestMulti;
  UPROPERTY(EditAnywhere)
  float BaseDebtInterestPerDayMulti;
  UPROPERTY(EditAnywhere, SaveGame)
  float DebtLimitMulti;
  UPROPERTY(EditAnywhere, SaveGame)
  float DebtPaymentRequiredMulti;  // * How much of the debt needs to be paid on payment days.
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ADebtManager : public AInfo {
  GENERATED_BODY()

public:
  ADebtManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  const class ADayManager* DayManager;
  UPROPERTY(EditAnywhere)
  const class AMarketEconomy* MarketEconomy;
  UPROPERTY(EditAnywhere)
  const class AStatisticsGen* StatisticsGen;

  UPROPERTY(EditAnywhere)
  FDebtManagerParams DebtManagerParams;

  UPROPERTY(EditAnywhere, SaveGame)
  FDebtManagerBehaviorParams BehaviorParams;

  UPROPERTY(EditAnywhere)
  class AStore* Store;

  UPROPERTY(EditAnywhere, SaveGame)
  float AdditionalDebt;
  UPROPERTY(EditAnywhere, SaveGame)
  float InterestAccrued;

  auto GetInterestRate() const -> float;
  auto GetInterest(float Amount) const -> float;
  auto GetDebtLimit() const -> float;
  auto GetPayableDebt() const -> float;
  auto GetMaxDebt() const -> float;

  void IssueDebt(float Amount);
  auto TryPayDebt() -> bool;

  UPROPERTY(EditAnywhere)
  FUpgradeable Upgradeable;
  void ChangeBehaviorParam(const TMap<FName, float>& ParamValues);
};