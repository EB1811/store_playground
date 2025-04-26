// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "UpgradeListWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UUpgradeListWidget : public UUserWidget {
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
  class UTextBlock* AvailableUpgradePointsText;
  UPROPERTY(meta = (BindWidget))
  class UWrapBox* UpgradeListPanelWrapBox;
  UPROPERTY(meta = (BindWidget))
  class UWrapBox* ReqsNotMetPanelWrapBox;

  UPROPERTY()
  EUpgradeClass UpgradeClass;

  std::function<void(FName)> SelectUpgradeFunc;

  void SetUpgradeClass(EUpgradeClass NewUpgradeClass, const FText& Title, const FText& Description);
  void RefreshUpgradeListUI();
};