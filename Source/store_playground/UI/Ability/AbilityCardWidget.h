// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "AbilityCardWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UAbilityCardWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY()
  FName AbilityId;

  UPROPERTY(meta = (BindWidget))
  class UImage* AbilityIconImage;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* AbilityNameText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* AbilityDescText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* AbilityCostText;
  UPROPERTY(meta = (BindWidget))
  class UButton* ActivateAbilityButton;

  UFUNCTION()
  void OnActivateAbilityButtonClicked();
  std::function<void(FName)> SelectAbilityFunc;
};