// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatisticsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UStatisticsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  const class AStatisticsGen* StatisticsGenRef;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TotalProfitText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* StoreStockValueText;

  void RefreshUI();
};