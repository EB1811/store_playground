// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "NegotiationSkillsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UNegotiationSkillsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* SkillsBox;
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UNegotiationSkillWidget> NegotiationSkillWidgetClass;

  void InitUI(TArray<FNegotiationSkill> NegotiationSkills);
};