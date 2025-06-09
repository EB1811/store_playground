// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "PopDetailsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UPopDetailsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UImage* Icon;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Name;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* WealthText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* PopulationText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ItemSpendText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ItemEconTypesText;
  UPROPERTY(meta = (BindWidget))
  class UBorder* BgBorder;
};