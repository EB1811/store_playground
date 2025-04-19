// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "AbilityListWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UAbilityListWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UAbilityCardWidget> AbilityCardWidgetClass;

  UPROPERTY(meta = (BindWidget))
  class UWrapBox* AbilityListPanelWrapBox;

  UPROPERTY()
  TArray<FEconEventAbility> EconEventAbilities;

  void RefreshUI();

  std::function<void(FName)> SelectAbilityFunc;
};