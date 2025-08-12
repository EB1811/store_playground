// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "PowerUpgradesWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UPowerUpgradesWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UUpgradeCardWidget> UpgradeCardWidgetClass;
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UDisabledUpgradeCardWidget> DisabledUpgradeCardWidgetClass;
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UDisabledUpgradeCardWidget> UnlockedUpgradeCardWidgetClass;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TitleText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* DescText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* AvailableUpgradePointsText;
  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* UpgradableUpgradesBox;
  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* LockedUpgradesBox;
  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* UnlockedUpgradesBox;

  UPROPERTY(EditAnywhere)
  class USoundBase* SelectSound;
  UPROPERTY(EditAnywhere)
  class USoundBase* UnlockSound;

  UPROPERTY(EditAnywhere)
  class AUpgradeManager* UpgradeManager;

  UPROPERTY(EditAnywhere)
  EUpgradeClass UpgradeClass;

  UPROPERTY(EditAnywhere)
  FName SelectedUpgradeID;
  UPROPERTY(EditAnywhere)
  class UUpgradeCardWidget* SelectedUpgradeCardWidget;

  void SelectUpgrade(FName UpgradeID, UUpgradeCardWidget* UpgradeCardWidget);
  void SelectNextUpgrade(FVector2D Direction);

  UFUNCTION()
  void UnlockUpgrade();

  void RefreshUI();
  void InitUI(EUpgradeClass _UpgradeClass, class AUpgradeManager* _UpgradeManager);
};