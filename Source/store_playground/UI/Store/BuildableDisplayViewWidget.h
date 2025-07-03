// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "BuildableDisplayViewWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UBuildableDisplayViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UBuildableDisplayWidget* BuildableDisplayWidget;

  void RefreshUI();
  void InitUI(FInUIInputActions InUIInputActions,
              class ABuildable* Buildable,
              class AStore* _Store,
              std::function<void()> _CloseWidgetFunc);
};