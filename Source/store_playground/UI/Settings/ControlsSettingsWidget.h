// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "ControlsSettingsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UControlsSettingsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(EditAnywhere)
  TSubclassOf<class UControlsRebindWidget> ControlsRebindWidgetClass;

  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* InGameControlsPanel;
  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* InUIControlsPanel;
  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* InCutsceneControlsPanel;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ErrorText;
  UPROPERTY(meta = (BindWidget))
  class UButton* ApplyButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* ResetButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* BackButton;

  class ASettingsManager* SettingsManager;

  UFUNCTION()
  void Apply();
  UFUNCTION()
  void Reset();
  UFUNCTION()
  void Back();

  void RebindKey(FPlayerKeyMapping KeyMapping, FInputChord InputChord);
  void ResetKey(FPlayerKeyMapping KeyMapping);

  void RefreshUI();
  void InitUI(FInUIInputActions _InUIInputActions,
              class ASettingsManager* _SettingsManager,
              std::function<void()> _BackFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  std::function<void()> BackFunc;
};