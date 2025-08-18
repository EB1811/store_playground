// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "NpcStoreViewWidget.generated.h"

UENUM()
enum class ETradeType : uint8 {
  Buy UMETA(DisplayName = "Buy"),
  Sell UMETA(DisplayName = "Sell"),
};

UCLASS()
class STORE_PLAYGROUND_API UNpcStoreViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UMenuHeaderWidget* MenuHeaderWidget;
  UPROPERTY(meta = (BindWidget))
  class URightSlideWidget* MoneySlideWidget;
  UPROPERTY(meta = (BindWidget))
  class UItemsWidget* ItemsWidget;
  UPROPERTY(meta = (BindWidget))
  class UTradeConfirmWidget* TradeConfirmWidget;

  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* SortByPriceButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* SortByNameButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* TradeButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* SwitchTradeTypeButton;
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
  class USoundBase* GeneralTradeSound;
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  class USoundBase* TradeConfirmSound;

  UPROPERTY(EditAnywhere)
  const class AMarketEconomy* MarketEconomy;
  UPROPERTY(EditAnywhere)
  const class AMarket* Market;
  UPROPERTY(EditAnywhere)
  const class AStatisticsGen* StatisticsGen;

  UPROPERTY(EditAnywhere)
  ETradeType TradeType;

  UPROPERTY(EditAnywhere)
  class AStore* Store;
  UPROPERTY(EditAnywhere)
  class UNpcStoreComponent* NpcStoreC;
  UPROPERTY(EditAnywhere)
  class UInventoryComponent* StoreInventory;
  UPROPERTY(EditAnywhere)
  class UInventoryComponent* PlayerInventory;

  UPROPERTY(EditAnywhere)
  bool bIsConfirming;

  void TradeConfirmed(int32 Quantity);

  UFUNCTION()
  void SortByPrice();
  UFUNCTION()
  void SortByName();
  UFUNCTION()
  void Trade();
  UFUNCTION()
  void SwitchTradeType();
  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInUIInputActions InUIInputActions,
              const class AMarketEconomy* _MarketEconomy,
              const class AMarket* _Market,
              const class AStatisticsGen* _StatisticsGen,
              class AStore* _Store,
              class UNpcStoreComponent* _NpcStoreC,
              class UInventoryComponent* StoreInventoryC,
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