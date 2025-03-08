// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StockCheckWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UStockCheckWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class UInventoryWidget* PlayerInventoryWidget;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* BaseItemName;

  UPROPERTY(meta = (BindWidget))
  class UButton* ShowItemButton;

  void InitStockCheckUI(class UInventoryComponent* PlayerInventory, const class UItemBase* BaseItem);

  UFUNCTION()
  void OnShowItemButtonClicked();

  std::function<void(class UItemBase*, class UInventoryComponent* FromInventory)> ShowItemFunc;
};