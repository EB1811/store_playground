// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "NewsAndEconomyViewWidget.generated.h"

UENUM()
enum class ENewsAndEconomyViewTab : uint8 {
  Articles UMETA(DisplayName = "Articles"),
  Economy UMETA(DisplayName = "Economy"),
};
ENUM_RANGE_BY_COUNT(ENewsAndEconomyViewTab, 2);

UCLASS()
class STORE_PLAYGROUND_API UNewsAndEconomyViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UMenuHeaderWidget* MenuHeaderWidget;
  UPROPERTY(meta = (BindWidget))
  class UEconomyDetailsWidget* EconomyDetailsWidget;

  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* BackButton;

  UPROPERTY(EditAnywhere)
  const class ANewsGen* NewsGen;
  UPROPERTY(EditAnywhere)
  const class AMarketEconomy* MarketEconomy;

  UPROPERTY(EditAnywhere)
  ENewsAndEconomyViewTab ActiveTab;
  void SwitchTab(ENewsAndEconomyViewTab Tab);

  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInputActions InputActions,
              const class ANewsGen* _NewsGen,
              const class AMarketEconomy* _MarketEconomy,
              std::function<void()> _CloseWidgetFunc);

  std::function<void()> CloseWidgetFunc;
};