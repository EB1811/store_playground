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
  class UVerticalBox* AvailableAbilityListBox;
  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* NotEnoughMoneyAbilityListBox;
  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* UnavailableAbilityListBox;

  UPROPERTY(EditAnywhere)
  TArray<FEconEventAbility> AvailableAbilities;
  UPROPERTY(EditAnywhere)
  TArray<FEconEventAbility> UnavailableAbilities;
  UPROPERTY(EditAnywhere)
  TArray<FEconEventAbility> NotEnoughMoneyAbilities;

  void RefreshUI();
  void InitUI(TArray<FEconEventAbility> _AvailableAbilities,
              TArray<FEconEventAbility> _NotEnoughMoneyAbilities,
              TArray<FEconEventAbility> _UnavailableAbilities,
              std::function<void(FName)> _SelectAbilityFunc);

  std::function<void(FName)> SelectAbilityFunc;
};