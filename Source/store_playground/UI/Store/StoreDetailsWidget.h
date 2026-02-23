// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "StoreDetailsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UStoreDetailsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UStoreDetailCardWidget* DayCardWidget;
  UPROPERTY(meta = (BindWidget))
  class UStoreDetailCardWidget* DebtCardWidget;
  UPROPERTY(meta = (BindWidget))
  class UStoreMonetaryDetailCardWidget* ProfitCardWidget;
  UPROPERTY(meta = (BindWidget))
  class UStoreMonetaryDetailCardWidget* NetWorthCardWidget;
  UPROPERTY(meta = (BindWidget))
  class UStoreDetailCardWidget* UpgradesCardWidget;

  UPROPERTY(meta = (BindWidget))
  class UStoreStatsGraphsWidget* StatsGraphsWidget;

  UPROPERTY(EditAnywhere)
  const class ADayManager* DayManager;
  UPROPERTY(EditAnywhere)
  const class AStorePhaseManager* StorePhaseManager;
  UPROPERTY(EditAnywhere)
  const class AMarketEconomy* MarketEconomy;
  UPROPERTY(EditAnywhere)
  const class AMarket* Market;
  UPROPERTY(EditAnywhere)
  const class ADebtManager* DebtManager;
  UPROPERTY(EditAnywhere)
  const class AUpgradeManager* UpgradeManager;
  UPROPERTY(EditAnywhere)
  const class AAbilityManager* AbilityManager;
  UPROPERTY(EditAnywhere)
  const class UInventoryComponent* PlayerInventoryC;

  UPROPERTY(EditAnywhere)
  class AStatisticsGen* StatisticsGen;
  UPROPERTY(EditAnywhere)
  class AStore* Store;

  void RefreshUI();
  void InitUI(const class ADayManager* _DayManager,
              const class AStorePhaseManager* _StorePhaseManager,
              const class AMarketEconomy* _MarketEconomy,
              const class AMarket* _Market,
              const class ADebtManager* _DebtManager,
              const class AUpgradeManager* _UpgradeManager,
              const class AAbilityManager* _AbilityManager,
              const class UInventoryComponent* _PlayerInventoryC,
              class AStatisticsGen* _StatisticsGen,
              class AStore* _Store);

  UPROPERTY(EditAnywhere)
  FTimerHandle RefreshTimerHandle;
  void RefreshTick();
};