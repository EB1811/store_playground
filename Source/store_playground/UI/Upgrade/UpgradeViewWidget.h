// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "UpgradeViewWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UUpgradeViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UPowerUpgradesWidget* Power1UpgradeWidget;
  UPROPERTY(meta = (BindWidget))
  class UPowerUpgradesWidget* Power2UpgradeWidget;
  UPROPERTY(meta = (BindWidget))
  class UPowerUpgradesWidget* Power3UpgradeWidget;
  UPROPERTY(meta = (BindWidget))
  class UPowerUpgradesWidget* Power4UpgradeWidget;

  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* UnlockButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* BackButton;

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* ShowAnim;
  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* HideAnim;
  UPROPERTY(EditAnywhere)
  class USoundBase* OpenSound;
  UPROPERTY(EditAnywhere)
  class USoundBase* HideSound;

  UPROPERTY(EditAnywhere)
  class AUpgradeManager* UpgradeManager;

  UPROPERTY(EditAnywhere)
  class UPowerUpgradesWidget* RelevantPowerUpgradesWidget;

  UPROPERTY()
  EUpgradeClass UpgradeClass;

  UFUNCTION()
  void Unlock();
  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInUIInputActions InUIInputActions,
              class AUpgradeManager* _UpgradeManager,
              class UUpgradeSelectComponent* UpgradeSelectC,
              std::function<void()> _CloseWidgetFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  UPROPERTY(EditAnywhere)
  FUIBehaviour UIBehaviour;
  void SetupUIBehaviour();

  std::function<void()> CloseWidgetFunc;
};