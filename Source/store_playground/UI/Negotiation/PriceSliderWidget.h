// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "PriceSliderWidget.generated.h"

// UI params
USTRUCT()
struct FPriceSliderUIParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float AcceptanceBarMulti;  // Determines the max and min values, and thus scale.
};

UCLASS()
class STORE_PLAYGROUND_API UPriceSliderWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UProgressBar* RedBar;
  UPROPERTY(meta = (BindWidget))
  class UProgressBar* YellowBar;
  UPROPERTY(meta = (BindWidget))
  class UProgressBar* GreenBar;
  UPROPERTY(meta = (BindWidget))
  class USlider* MarketPriceSlider;
  UPROPERTY(meta = (BindWidget))
  class USlider* BoughtAtSlider;
  UPROPERTY(meta = (BindWidget))
  class USizeBox* YouTopBox;
  UPROPERTY(meta = (BindWidget))
  class USizeBox* YouBottomBox;
  UPROPERTY(meta = (BindWidget))
  class USizeBox* ThemTopBox;
  UPROPERTY(meta = (BindWidget))
  class USizeBox* ThemBottomBox;
  UPROPERTY(meta = (BindWidget))
  class USlider* NPCPriceSlider;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NPCPriceText;
  UPROPERTY(meta = (BindWidget))
  class USlider* PlayerPriceSlider;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* PlayerPriceText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* AcceptanceChanceText;
  UPROPERTY(meta = (BindWidget))
  class UHorizontalBox* PlayerSliderBox;  // * To get size of the slider.

  UPROPERTY(EditAnywhere)
  class USoundBase* PriceChangeSound;

  UPROPERTY(EditAnywhere)
  FPriceSliderUIParams PriceSliderUIParams;
  UPROPERTY(EditAnywhere)
  FLinearColor PriceNormalColor;
  UPROPERTY(EditAnywhere)
  FLinearColor PriceErrorColor;  // * e.g., when the price is too high for the player to afford.
  UPROPERTY(EditAnywhere)
  FLinearColor NpcPriceNormalColor;

  UPROPERTY(EditAnywhere)
  const class UNegotiationSystem* NegotiationSystem;
  UPROPERTY(EditAnywhere)
  const class ACustomerAIManager* CustomerAIManager;

  UPROPERTY(EditAnywhere)
  NegotiationType Type;
  UPROPERTY(EditAnywhere)
  float MaxValue;
  UPROPERTY(EditAnywhere)
  float MinValue;
  UPROPERTY(EditAnywhere)
  float PlayerMoney;

  UPROPERTY(EditAnywhere)
  bool bInstantAcceptanceChange;
  UPROPERTY(EditAnywhere)
  float AcceptanceChanceUpdateFrequency;
  UPROPERTY(EditAnywhere)
  float LastAcceptanceChanceCheckTime;  // * To limit how often the acceptance chance text is updated.

  UPROPERTY(EditAnywhere)
  bool bPreciseInput;  // * Increment the price by 1.
  void TogglePreciseInput();

  UFUNCTION()
  void UpdatePlayerPriceText(float Value);

  void ChangePrice(float Direction);
  void UpdateNegotiationPrices(float NpcAcceptance,
                               float MarketPrice,
                               float PlayerPrice,
                               float NpcPrice,
                               int32 HagglingCount);

  // ? Turn into a struct?
  void InitUI(const class UNegotiationSystem* _NegotiationSystem,
              const class ACustomerAIManager* _CustomerAIManager,
              NegotiationType _Type,
              float NpcAcceptance,
              float MarketPrice,
              float PlayerPrice,
              float NpcPrice,
              float _PlayerMoney,
              float BoughtAtPrice = 0.0f);
};