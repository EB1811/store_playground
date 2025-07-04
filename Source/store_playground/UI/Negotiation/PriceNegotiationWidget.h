// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "PriceNegotiationWidget.generated.h"

// TODO: Error handling.

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
  UPROPERTY(meta = (BindWidget))
  class UControlsHelpersWidget* ControlsHelpersWidget;

  UPROPERTY(EditAnywhere)
  const class AStore* Store;

  UPROPERTY(EditAnywhere)
  class UNegotiationSystem* NegotiationSystem;

  void OfferAcceptPrice();

  void RefreshUI();
  void InitUI(FInUIInputActions InUIInputActions,
              const class AStore* _Store,
              class UNegotiationSystem* _NegotiationSystem,
              std::function<void(float)> _OfferAcceptFunc,
              std::function<void()> _RejectFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  std::function<void(float)> OfferAcceptFunc;
  std::function<void()> RejectFunc;
};