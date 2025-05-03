// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmptySaveSlotWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UEmptySaveSlotWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY()
  int32 SlotIndex;

  UPROPERTY(meta = (BindWidget))
  class UButton* SelectButton;  // Load or save button.

  UFUNCTION()
  void OnSelectButtonClicked();
  std::function<void(int32)> SelectSlotFunc;
};