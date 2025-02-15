// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NegotiationWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UNegotiationWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UButton* OfferButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* AcceptButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* RejectButton;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ItemName;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ItemQuantity;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* BasePrice;
  UPROPERTY(meta = (BindWidget))
  class UImage* ItemIcon;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NegotiationStateText;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* OfferedPrice;
  UPROPERTY(meta = (BindWidget))
  class USlider* PlayerOfferedPrice;

  UPROPERTY(EditAnywhere)
  class UNegotiationSystem* NegotiationRef;

  void RefreshNegotiationWhole();
  void RefreshNegotiationState();

  UFUNCTION()
  void OnOfferButtonClicked();
  UFUNCTION()
  void OnAcceptButtonClicked();
  UFUNCTION()
  void OnRejectButtonClicked();

  std::function<void()> CloseNegotiationUI;
};