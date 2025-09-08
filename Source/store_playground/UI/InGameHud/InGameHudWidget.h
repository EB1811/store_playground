// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "InGameHudWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UInGameHudWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class ULeftSlideWidget* UpgradePointsSlideWidget;
  UPROPERTY(meta = (BindWidget))
  class ULeftSlideWidget* NotificationsSlideWidget;
  UPROPERTY(meta = (BindWidget))
  class UControlsHelpersWidget* ControlsHelpersWidget;
  UPROPERTY(meta = (BindWidget))
  class URightSlideWidget* DaySlideWidget;
  UPROPERTY(meta = (BindWidget))
  class URightSlideSecondaryWidget* OwnedSlideWidget;
  UPROPERTY(meta = (BindWidget))
  class URightSlideSecondaryWidget* MoneySlideWidget;
  UPROPERTY(meta = (BindWidget))
  class URightSlideInvertedWidget* LocationSlideWidget;

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* ShowAnim;
  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* HideAnim;

  UPROPERTY(EditAnywhere)
  const class AUpgradeManager* UpgradeManager;
  UPROPERTY(EditAnywhere)
  const class ADayManager* DayManager;
  UPROPERTY(EditAnywhere)
  const class AStorePhaseManager* StorePhaseManager;
  UPROPERTY(EditAnywhere)
  const class AStore* Store;
  UPROPERTY(EditAnywhere)
  const class ALevelManager* LevelManager;

  UPROPERTY(EditAnywhere)
  FInGameInputActions InGameInputActions;

  UPROPERTY(EditAnywhere)
  class USoundBase* NotifySound;
  UPROPERTY(EditAnywhere)
  bool bNeedUpgradePointsNotify;  // * If Notify is called shile hidden, need to perform notification upon showing.
  void NotifyUpgradePointsGained();
  UPROPERTY(EditAnywhere)
  FTimerHandle HideTimerHandle;
  UFUNCTION()
  void HideUpgradePointsNotify();

  void RefreshUI();
  void InitUI(FInGameInputActions _InGameInputActions);

  UPROPERTY(EditAnywhere)
  bool bShowingHud;  // ! Workaround for HideAnimComplete event stuck being called when playing animations fast.
  void Show();
  void Hide();
  UFUNCTION()
  void HideAnimComplete();
};