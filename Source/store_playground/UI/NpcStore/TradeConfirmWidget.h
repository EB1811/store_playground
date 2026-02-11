// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NpcStoreViewWidget.h"
#include "TradeConfirmWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UTradeConfirmWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TitleText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Name;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* QuantityText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TradePrice;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* HaveNumberText;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ErrorText;  // Temp

  UPROPERTY(meta = (BindWidget))
  class UButton* IncreaseQuantityButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* DecreaseQuantityButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* ConfirmTradeButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* BackButton;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  class USoundBase* ChangeQuantitySound;

  UPROPERTY()
  ETradeType TradeType;

  UPROPERTY()
  int32 Quantity;
  UPROPERTY()
  int32 AvailableQuantity;
  UPROPERTY()
  bool bQuantityAffectsBuy;
  UPROPERTY()
  float Money;
  UPROPERTY()
  float AvailableMoney;

  std::function<float()> ShowPriceFunc;

  void ChangeQuantity(float Direction);
  UFUNCTION()
  void IncreaseQuantity();
  UFUNCTION()
  void DecreaseQuantity();
  UFUNCTION()
  void ConfirmTrade();
  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(ETradeType _TradeType,
              const FText& ItemName,
              int32 _AvailableQuantity,
              bool _bQuantityAffectsBuy,
              float _Money,
              float _AvailableMoney,
              std::function<float()> _ShowPriceFunc,
              std::function<void(int32)> _ConfirmTradeFunc,
              std::function<void()> _BackFunc);

  std::function<void(int32)> ConfirmTradeFunc;
  std::function<void()> BackFunc;
};