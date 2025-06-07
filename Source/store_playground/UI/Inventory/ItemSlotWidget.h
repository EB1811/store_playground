// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlotWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UItemSlotWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UImage* Icon;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Name;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Quantity;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ShowPrice;
  UPROPERTY(meta = (BindWidget))
  class UButton* SelectItemButton;

  UPROPERTY(EditAnywhere)
  const class UItemBase* ItemRef;

  UPROPERTY(EditAnywhere)
  bool bIsSelected;

  std::function<float(FName)> ShowPriceFunc;
  void RefreshUI();

  UFUNCTION()
  void OnSelectItemButtonClicked();
  std::function<void(const class UItemBase*)> OnSelectItemFunc;
};