// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UAbilityWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  class AAbilityManager* AbilityManagerRef;

  UPROPERTY(meta = (BindWidget))
  class UAbilityListWidget* AbilityListWidget;
  // UPROPERTY(meta = (BindWidget))
  // class UAbilitySelectWidget* AbilitySelectWidget;

  void RefreshUI();
};