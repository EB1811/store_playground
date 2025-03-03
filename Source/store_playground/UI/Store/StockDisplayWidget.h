// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StockDisplayWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UStockDisplayWidget : public UUserWidget {
  GENERATED_BODY()

public:
  // virtual void NativeOnInitialized() override;
  // virtual void NativeConstruct() override;

  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class UPlayerAndContainerWidget* PlayerAndContainerWidget;

  // TODO: Add stock display details.
};