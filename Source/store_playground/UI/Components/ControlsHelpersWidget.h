// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControlsHelpersWidget.generated.h"

USTRUCT()
struct FControls {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FText ActionName;
  UPROPERTY(EditAnywhere)
  class UInputAction* ActionBinding;
};

UCLASS()
class STORE_PLAYGROUND_API UControlsHelpersWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UControlTextWidget> ControlTextWidgetClass;

  UPROPERTY(meta = (BindWidget))
  class UWrapBox* ControlsWrapBox;

  UPROPERTY(EditAnywhere)
  TArray<FControls> Controls;

  void SetComponentUI(const TArray<FControls>& _Controls);
};