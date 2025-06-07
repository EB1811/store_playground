// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActionWidget.h"
#include "ControlMenuButtonWidget.generated.h"

// * Both buttons and control

UCLASS()
class STORE_PLAYGROUND_API UControlMenuButtonWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ActionText;
  UPROPERTY(meta = (BindWidget))
  UCommonActionWidget* CommonActionWidget;
  UPROPERTY(meta = (BindWidget))
  class UButton* ControlButton;
};