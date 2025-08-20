// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "StoreStatsGraphsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UStoreStatsGraphsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UStoreStatsGraphWidget* DayProfitGraphWidget;
  UPROPERTY(meta = (BindWidget))
  class UStoreStatsGraphWidget* NetWorthGraphWidget;

  UPROPERTY(EditAnywhere)
  const class AStatisticsGen* StatisticsGen;

  void RefreshUI();
  void InitUI(const class AStatisticsGen* _StatisticsGen);
};