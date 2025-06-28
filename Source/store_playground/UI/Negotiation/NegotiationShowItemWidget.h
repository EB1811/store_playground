// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NegotiationShowItemWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UNegotiationShowItemWidget : public UUserWidget {
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
  class UControlMenuButtonWidget* SortByPriceButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* SortByNameButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* ShowButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* RejectButton;

  UPROPERTY(EditAnywhere)
  const class AStore* Store;
  UPROPERTY(EditAnywhere)
  const class AMarketEconomy* MarketEconomy;

  UPROPERTY(EditAnywhere)
  class UInventoryComponent* InventoryC;
  UPROPERTY(EditAnywhere)
  class UNegotiationSystem* NegotiationSystem;

  UFUNCTION()
  void SortByPrice();
  UFUNCTION()
  void SortByName();
  UFUNCTION()
  void Show();
  UFUNCTION()
  void Reject();

  void RefreshUI();
  void InitUI(const class AStore* _Store,
              const class AMarketEconomy* _MarketEconomy,
              class UInventoryComponent* _InventoryC,
              class UNegotiationSystem* _NegotiationSystem,
              std::function<void(class UItemBase*)> _ShowFunc,
              std::function<void()> _RejectFunc);

  std::function<void(class UItemBase*)> ShowFunc;
  std::function<void()> RejectFunc;
};