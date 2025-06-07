// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemDetailsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UItemDetailsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Name;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* EconInfo;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Desc;
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

  UPROPERTY(EditAnywhere)
  const class UItemBase* ItemRef;

  std::function<float(FName)> ShowPriceFunc;
  std::function<float(FName)> MarketPriceFunc;

  void RefreshUI();
  void InitUI(const class UItemBase* _ItemRef,
              FName _ShowPriceText,
              std::function<float(FName)> _MarketPriceFunc,
              std::function<float(FName)> _ShowPriceFunc = nullptr);
};