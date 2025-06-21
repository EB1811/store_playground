// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "SaveLoadConfirmWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API USaveLoadConfirmWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Text;

  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* ConfirmButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* CancelButton;

  UFUNCTION()
  void Confirm();
  UFUNCTION()
  void Cancel();

  void InitUI(FText _Text, std::function<void()> _ConfirmFunc, std::function<void()> _CancelFunc);

  std::function<void()> ConfirmFunc;
  std::function<void()> CancelFunc;
};