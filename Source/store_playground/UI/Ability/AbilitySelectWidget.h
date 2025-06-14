// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "AbilitySelectWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UAbilitySelectWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Name;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* CostText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* CooldownText;
};