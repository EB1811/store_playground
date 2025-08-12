// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "AbilityViewWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UAbilityViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UAbilityListWidget* AbilityListWidget;
  UPROPERTY(meta = (BindWidget))
  class UAbilitySelectWidget* AbilitySelectWidget;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* ActivateButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* SelectReplaceAbilityButton;
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
  class USoundBase* SelectReplaceSound;
  UPROPERTY(EditAnywhere)
  class USoundBase* ActivateSound;

  UPROPERTY(EditAnywhere)
  const class AStore* Store;

  UPROPERTY(EditAnywhere)
  FInUIInputActions InUIInputActions;

  UPROPERTY(EditAnywhere)
  class AAbilityManager* AbilityManager;

  UPROPERTY(EditAnywhere)
  FName SelectedAbilityID;
  UPROPERTY(EditAnywhere)
  bool bShowingAbilitySelect;

  UFUNCTION()
  void Activate();
  UFUNCTION()
  void SelectReplaceAbility();
  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInUIInputActions _InUIInputActions,
              const class AStore* _Store,
              class AAbilityManager* _AbilityManager,
              std::function<void()> _CloseWidgetFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  UPROPERTY(EditAnywhere)
  FUIBehaviour UIBehaviour;
  void SetupUIBehaviour();

  std::function<void()> CloseWidgetFunc;
};