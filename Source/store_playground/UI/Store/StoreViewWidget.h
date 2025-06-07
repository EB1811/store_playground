// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "Types/SlateEnums.h"
#include "StoreViewWidget.generated.h"

UENUM()
enum class EStoreViewTab : uint8 {
  Details UMETA(DisplayName = "Details"),
  Expansion UMETA(DisplayName = "Expansion"),
};
ENUM_RANGE_BY_COUNT(EStoreViewTab, 2);

UCLASS()
class STORE_PLAYGROUND_API UStoreViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UMenuHeaderWidget* MenuHeaderWidget;
  UPROPERTY(meta = (BindWidget))
  class UStoreDetailsWidget* StoreDetailsWidget;

  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* BackButton;

  UPROPERTY(EditAnywhere)
  EStoreViewTab ActiveTab;
  void SwitchTab(EStoreViewTab Tab);

  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInputActions InputActions,
              const class ADayManager* DayManager,
              const class AStorePhaseManager* StorePhaseManager,
              const class AMarketEconomy* MarketEconomy,
              const class AMarket* Market,
              const class AStatisticsGen* StatisticsGen,
              const class AUpgradeManager* UpgradeManager,
              const class AAbilityManager* AbilityManager,
              const class UInventoryComponent* PlayerInventoryC,
              class AStore* Store,
              class AStoreExpansionManager* StoreExpansionManager,
              std::function<void()> _CloseWidgetFunc);

  std::function<void()> CloseWidgetFunc;
};