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

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NameText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* DescText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* CostText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* CooldownText;
  UPROPERTY(meta = (BindWidget))
  class UButton* SelectButton;

  UPROPERTY()
  FName AbilityId;

  void InitUI(FEconEventAbility Ability, std::function<void(FName)> _SelectAbilityFunc);

  UFUNCTION()
  void OnSelectButtonClicked();
  std::function<void(FName)> SelectAbilityFunc;
};