// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CompactItemDetailsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UCompactItemDetailsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Name;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* EconInfo;
  UPROPERTY(meta = (BindWidget))
  class UImage* Icon;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Quantity;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ShowPriceText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ShowPrice;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* MarketPrice;

  void InitUI(const class UItemBase* Item, FName _ShowPriceText, float _MarketPrice, float _ShowPrice = 0.0f);
};