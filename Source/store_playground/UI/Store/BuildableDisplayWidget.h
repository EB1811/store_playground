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

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* PriceText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* MoneyNumberText;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* BuildStockDisplayButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* BackButton;

  UPROPERTY()
  class ABuildable* Buildable;
  UPROPERTY(EditAnywhere)
  class AStore* Store;

  void SetBuildable(class ABuildable* Buildable);

  UFUNCTION()
  void BuildStockDisplay();
  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(class ABuildable* _Buildable, class AStore* _Store, std::function<void()> _CloseWidgetFunc);

  std::function<void()> CloseWidgetFunc;
};