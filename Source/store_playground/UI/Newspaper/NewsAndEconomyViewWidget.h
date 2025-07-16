// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
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

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* ShowAnim;
  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* HideAnim;
  UPROPERTY(EditAnywhere)
  class USoundBase* OpenSound;
  UPROPERTY(EditAnywhere)
  class USoundBase* HideSound;

  UPROPERTY(EditAnywhere)
  ENewsAndEconomyViewTab ActiveTab;
  void SwitchTab(ENewsAndEconomyViewTab Tab);

  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInUIInputActions InUIInputActions,
              const class ADayManager* _DayManager,
              const class AMarketEconomy* _MarketEconomy,
              const class AStatisticsGen* _StatisticsGen,
              class ANewsGen* _NewsGen,
              std::function<void()> _CloseWidgetFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  UPROPERTY(EditAnywhere)
  FUIBehaviour UIBehaviour;
  void SetupUIBehaviour();

  std::function<void()> CloseWidgetFunc;
};