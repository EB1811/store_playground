// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildableDisplayWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UBuildableDisplayWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeConstruct() override;

  UPROPERTY()
  const class ABuildable* BuildableRef;

  UPROPERTY(meta = (BindWidget))
  class UButton* BuildStockDisplayButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* BuildDecorationButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* CancelButton;

  void SetBuildable(class ABuildable* Buildable);

  UFUNCTION()
  void OnStockDisplayButtonClicked();
  UFUNCTION()
  void OnDecorationButtonClicked();
  UFUNCTION()
  void OnCancelButtonClicked();

  std::function<bool(class ABuildable* Buildable)> BuildStockDisplayFunc;
  std::function<bool(class ABuildable* Buildable)> BuildDecorationFunc;

  std::function<void()> CloseWidgetFunc;
};