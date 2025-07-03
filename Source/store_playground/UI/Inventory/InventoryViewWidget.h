// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "InventoryViewWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UInventoryViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UMenuHeaderWidget* MenuHeaderWidget;
  UPROPERTY(meta = (BindWidget))
  class URightSlideWidget* MoneySlideWidget;
  UPROPERTY(meta = (BindWidget), EditAnywhere)
  class ULeftSlideWidget* ItemsValueSlideWidget;
  UPROPERTY(meta = (BindWidget), EditAnywhere)
  class UItemsWidget* ItemsWidget;

  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* SortByMarketPriceButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* SortByNameButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* BackButton;

  UPROPERTY(EditAnywhere)
  const class AStore* Store;
  UPROPERTY(EditAnywhere)
  const class AMarketEconomy* MarketEconomy;

  UFUNCTION()
  void SortByMarketPrice();
  UFUNCTION()
  void SortByName();
  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInUIInputActions InUIInputActions,
              const class AStore* _Store,
              const class AMarketEconomy* _MarketEconomy,
              const class UInventoryComponent* InventoryC,
              std::function<void()> _CloseWidgetFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  std::function<void()> CloseWidgetFunc;
};