// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "StockDisplayViewWidget.generated.h"

UENUM()
enum class EStockDisplayViewType : uint8 {
  Player UMETA(DisplayName = "Items"),
  Display UMETA(DisplayName = "Display"),
};

UCLASS()
class STORE_PLAYGROUND_API UStockDisplayViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UMenuHeaderWidget* MenuHeaderWidget;
  UPROPERTY(meta = (BindWidget))
  class UItemsWidget* ItemsWidget;

  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* SortByMarketPriceButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* SortByNameButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* AddOrTakeButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* SwitchViewTypeButton;
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

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  class USoundBase* SwtichSound;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  class USoundBase* AddOrTakeSound;

  UPROPERTY(EditAnywhere)
  const class AMarketEconomy* MarketEconomy;
  UPROPERTY(EditAnywhere)
  const class AStatisticsGen* StatisticsGen;

  UPROPERTY(EditAnywhere)
  EStockDisplayViewType StockDisplayViewType;

  UPROPERTY(EditAnywhere)
  class AStore* Store;
  UPROPERTY(EditAnywhere)
  class UStockDisplayComponent* StockDisplayC;
  UPROPERTY(EditAnywhere)
  class UInventoryComponent* DisplayInventory;
  UPROPERTY(EditAnywhere)
  class UInventoryComponent* PlayerInventory;

  UFUNCTION()
  void SortByMarketPrice();
  UFUNCTION()
  void SortByName();
  UFUNCTION()
  void AddOrTake();
  UFUNCTION()
  void SwitchViewType();
  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInUIInputActions InUIInputActions,
              const class AMarketEconomy* _MarketEconomy,
              const class AStatisticsGen* _StatisticsGen,
              class AStore* _Store,
              class UStockDisplayComponent* _StockDisplayC,
              class UInventoryComponent* DisplayInventoryC,
              class UInventoryComponent* PlayerInventoryC,
              std::function<void()> _CloseWidgetFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  UPROPERTY(EditAnywhere)
  FUIBehaviour UIBehaviour;
  void SetupUIBehaviour();

  std::function<void()> CloseWidgetFunc;
};