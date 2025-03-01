// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryViewWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UInventoryViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;
  virtual void NativeConstruct() override;

  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class UTextBlock* StoreMoneyText;
  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class UInventoryWidget* PlayerInventoryWidget;
  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class UInventoryWidget* StoreInventoryWidget;

  UPROPERTY(EditAnywhere)
  class AStore* Store;

  // ? Maybe have tabs for different inventories.

  void RefreshInventoryViewUI();
};