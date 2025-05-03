// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "PopulatedSaveSlotWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UPopulatedSaveSlotWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY()
  int32 SlotIndex;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NameText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* LastModifiedText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* CurrentDayText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* StoreMoneyText;

  UPROPERTY(meta = (BindWidget))
  class UButton* SelectButton;  // Load or save button.

  void SetSlotData(int32 index, const FSaveSlotData& SlotData);

  UFUNCTION()
  void OnSelectButtonClicked();
  std::function<void(int32)> SelectSlotFunc;
};