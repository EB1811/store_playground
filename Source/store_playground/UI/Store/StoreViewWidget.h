// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
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
  class UStoreExpansionsListWidget* StoreExpansionsListWidget;

  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* UnlockButton;
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
  const class ALevelManager* LevelManager;

  UPROPERTY(EditAnywhere)
  EStoreViewTab ActiveTab;
  void SwitchTab(EStoreViewTab Tab);

  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInUIInputActions InUIInputActions,
              const class ALevelManager* _LevelManager,
              const class ADayManager* DayManager,
              const class AStorePhaseManager* StorePhaseManager,
              const class AMarketEconomy* MarketEconomy,
              const class AMarket* Market,
              const class AUpgradeManager* UpgradeManager,
              const class AAbilityManager* AbilityManager,
              const class UInventoryComponent* PlayerInventoryC,
              class AStatisticsGen* StatisticsGen,
              class AStore* Store,
              class AStoreExpansionManager* StoreExpansionManager,
              std::function<void()> _CloseWidgetFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  UPROPERTY(EditAnywhere)
  FUIBehaviour UIBehaviour;
  void SetupUIBehaviour();

  std::function<void()> CloseWidgetFunc;
};