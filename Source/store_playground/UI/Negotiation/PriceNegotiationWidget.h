// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PriceNegotiationWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UPriceNegotiationWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class URightSlideWidget* MoneySlideWidget;
  UPROPERTY(meta = (BindWidget))
  class UPriceSliderWidget* PriceSliderWidget;
  UPROPERTY(meta = (BindWidget))
  class UCompactItemDetailsWidget* CompactItemDetailsWidget;
  UPROPERTY(meta = (BindWidget))
  class UNegotiationSkillsWidget* NegotiationSkillsWidget;

  UPROPERTY(EditAnywhere)
  const class AStore* Store;

  UPROPERTY(EditAnywhere)
  class UNegotiationSystem* NegotiationSystem;

  void RefreshUI();
  void InitUI(const class AStore* _Store, class UNegotiationSystem* _NegotiationSystem);
};