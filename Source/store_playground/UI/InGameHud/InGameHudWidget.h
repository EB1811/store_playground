// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "InGameHudWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UInGameHudWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UNewsHudSlideWidget* NewsHudSlideWidget;
  UPROPERTY(meta = (BindWidget))
  class UControlsHelpersWidget* ControlsHelpersWidget;

  void InitUI(FInputActions InputActions);
  void RefreshUI();
};