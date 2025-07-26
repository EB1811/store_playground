// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "DisplaySettingsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UDisplaySettingsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* ResolutionComboBox;
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* WindowModeComboBox;
  UPROPERTY(meta = (BindWidget))
  class UCheckBox* VSyncCheckBox;
  UPROPERTY(meta = (BindWidget))
  class USlider* FrameRateLimitSlider;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* FrameRateLimitText;

  UPROPERTY(meta = (BindWidget))
  class UButton* ApplyButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* BackButton;

  class ASettingsManager* SettingsManager;

  UFUNCTION()
  void Apply();
  UFUNCTION()
  void Back();
  UFUNCTION()
  void OnFrameRateLimitChanged(float Value);

  void RefreshUI();
  void InitUI(FInUIInputActions _InUIInputActions,
              class ASettingsManager* _SettingsManager,
              std::function<void()> _BackFunc);

  std::function<void()> BackFunc;
};