// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Materials/MaterialInstance.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "EconomyDetailsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UEconomyDetailsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TotalWealthText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TotalBoughtText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TotalInflationText;
  UPROPERTY(meta = (BindWidget))
  class UStoreStatsGraphWidget* TotalWealthGraphWidget;
  UPROPERTY(meta = (BindWidget))
  class UStoreStatsGraphWidget* TotalBoughtGraphWidget;
  UPROPERTY(meta = (BindWidget))
  class UStoreStatsGraphWidget* TotalInflationGraphWidget;
  UPROPERTY(meta = (BindWidget))
  class UWrapBox* PopDetailsBox;
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UPopDetailsWidget> PopDetailsWidgetClass;

  UPROPERTY(EditAnywhere)
  TMap<EPopType, UMaterialInstance*> PopTypeMaterialMap;

  UPROPERTY(EditAnywhere)
  const class AMarketEconomy* MarketEconomy;
  UPROPERTY(EditAnywhere)
  const class AStatisticsGen* StatisticsGen;

  void RefreshUI();
  void InitUI(const class AMarketEconomy* _MarketEconomy, const class AStatisticsGen* _StatisticsGen);

  UPROPERTY(EditAnywhere)
  FTimerHandle RefreshTimerHandle;
  void RefreshTick();
};