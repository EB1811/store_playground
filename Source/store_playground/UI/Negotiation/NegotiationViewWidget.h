// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
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

  UPROPERTY(EditAnywhere)
  class USoundBase* OfferSound;
  UPROPERTY(EditAnywhere)
  class USoundBase* TradeSound;
  UPROPERTY(EditAnywhere)
  class USoundBase* LeaveSound;

  UPROPERTY(EditAnywhere)
  const class AAbilityManager* AbilityManager;
  UPROPERTY(EditAnywhere)
  const class AStore* Store;

  UPROPERTY(EditAnywhere)
  FInUIInputActions InUIInputActions;

  UPROPERTY(EditAnywhere)
  class UInventoryComponent* PlayerInventoryC;
  UPROPERTY(EditAnywhere)
  class UNegotiationSystem* NegotiationSystem;
  UPROPERTY(EditAnywhere)
  class UDialogueSystem* DialogueSystem;

  void ShowItem(class UItemBase* Item);
  void OfferAccept(float Price);
  void RejectLeave();

  void RefreshUI();
  void InitUI(FInUIInputActions _InUIInputActions,
              const class AAbilityManager* _AbilityManager,
              const class AStore* _Store,
              const class AMarketEconomy* _MarketEconomy,
              const class AStatisticsGen* _StatisticsGen,
              class UInventoryComponent* _PlayerInventoryC,
              class UNegotiationSystem* _NegotiationSystem,
              class UDialogueSystem* _DialogueSystem,
              std::function<void()> _CloseWidgetFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  std::function<void()> CloseWidgetFunc;
};