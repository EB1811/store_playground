// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActionWidget.h"
#include "MenuHeaderTabWidget.generated.h"

// * Both buttons and control

UCLASS()
class STORE_PLAYGROUND_API UMenuHeaderTabWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TabText;
  UPROPERTY(meta = (BindWidget))
  class UButton* TabButton;
  UPROPERTY(meta = (BindWidget))
  class UBorder* BottomBorder;

  UPROPERTY(EditAnywhere)
  int32 TabIndex;

  UFUNCTION()
  void OnTabClicked();
  std::function<void(int32 TabIndex)> OnTabClickedFunc;
};