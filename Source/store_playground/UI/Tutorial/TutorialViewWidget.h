// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/Tutorial/TutorialStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "TutorialViewWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UTutorialViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UTutorialStepWidget* TutorialStepWidget;
  UPROPERTY(meta = (BindWidget))
  class UScrollBox* StepsScrollBox;
  UPROPERTY(meta = (BindWidget))
  class UButton* NextButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* CloseButton;

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* ShowAnim;
  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* HideAnim;

  UPROPERTY(EditAnywhere)
  class USoundBase* NextSound;

  UPROPERTY(EditAnywhere)
  TArray<FUITutorialStep> TutorialSteps;
  UPROPERTY(EditAnywhere)
  int32 CurrentStepIndex;

  UPROPERTY(EditAnywhere)
  FTimerHandle VideoTimerHandle;

  UFUNCTION()
  void Next();
  UFUNCTION()
  void Close();

  void RefreshUI();
  void InitUI(FInUIInputActions _InUIInputActions,
              TArray<FUITutorialStep> _TutorialSteps,
              std::function<void()> _CloseWidgetFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  UPROPERTY(EditAnywhere)
  FUIBehaviour UIBehaviour;
  void SetupUIBehaviour();

  std::function<void()> CloseWidgetFunc;
};