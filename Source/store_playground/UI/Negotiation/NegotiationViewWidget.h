// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "NegotiationViewWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UNegotiationViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UDialogueWidget* DialogueWidget;
  UPROPERTY(meta = (BindWidget))
  class UPriceNegotiationWidget* PriceNegotiationWidget;
  UPROPERTY(meta = (BindWidget))
  class UNegotiationShowItemWidget* NegotiationShowItemWidget;
  UPROPERTY(meta = (BindWidget))
  class UControlsHelpersWidget* ControlsHelpersWidget;
  // Temp
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* OfferAcceptButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* RejectButton;

  UPROPERTY(EditAnywhere)
  class UInventoryComponent* PlayerInventoryC;
  UPROPERTY(EditAnywhere)
  class UNegotiationSystem* NegotiationSystem;
  UPROPERTY(EditAnywhere)
  class UDialogueSystem* DialogueSystem;

  void ShowItem(class UItemBase* Item);
  void OfferAccept();
  void Reject();

  // Temp
  UFUNCTION()
  void OfferAcceptClicked();
  UFUNCTION()
  void RejectClicked();

  void RefreshUI();
  void InitUI(FInputActions InputActions,
              const class AStore* _Store,
              const class AMarketEconomy* _MarketEconomy,
              class UInventoryComponent* _PlayerInventoryC,
              class UNegotiationSystem* _NegotiationSystem,
              class UDialogueSystem* _DialogueSystem,
              std::function<void()> _CloseWidgetFunc);

  std::function<void()> CloseWidgetFunc;
};