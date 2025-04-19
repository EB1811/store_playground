// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "AbilitySelectWidget.generated.h"

// ! Not used yet.

UCLASS()
class STORE_PLAYGROUND_API UAbilitySelectWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UUpgradeWidget> UpgradeWidgetClass;

  UPROPERTY(EditAnywhere)
  const class AUpgradeManager* UpgradeManagerRef;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* UpgradeListNameText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* UpgradeListDescText;
  UPROPERTY(meta = (BindWidget))
  class UWrapBox* UpgradeListPanelWrapBox;

  UPROPERTY()
  EUpgradeClass UpgradeClass;

  std::function<void(FName)> SelectUpgradeFunc;

  void SetUpgradeClass(EUpgradeClass NewUpgradeClass, const FText& Title, const FText& Description);
  void RefreshUpgradeListUI();
};