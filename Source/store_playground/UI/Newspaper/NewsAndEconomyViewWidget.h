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
  class UNewspaperWidget* NewspaperWidget;
  UPROPERTY(meta = (BindWidget))
  class UEconomyDetailsWidget* EconomyDetailsWidget;

  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* BackButton;

  UPROPERTY(EditAnywhere)
  ENewsAndEconomyViewTab ActiveTab;
  void SwitchTab(ENewsAndEconomyViewTab Tab);

  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInUIInputActions InUIInputActions,
              const class ADayManager* _DayManager,
              const class AMarketEconomy* _MarketEconomy,
              class ANewsGen* _NewsGen,
              std::function<void()> _CloseWidgetFunc);

  std::function<void()> CloseWidgetFunc;
};