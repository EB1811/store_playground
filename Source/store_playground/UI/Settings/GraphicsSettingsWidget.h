// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "GraphicsSettingsWidget.generated.h"

// todo-low: Profile hardware to auto set and suggest settings.

UCLASS()
class STORE_PLAYGROUND_API UGraphicsSettingsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  // Quality presets
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* OverallQualityComboBox;

  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* ViewDistanceComboBox;
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* ShadowQualityComboBox;
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* GlobalIlluminationComboBox;
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* ReflectionQualityComboBox;
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* AntiAliasingMethodComboBox;
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* AntiAliasingComboBox;
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* TextureQualityComboBox;
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* VisualEffectsComboBox;
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* PostProcessingComboBox;
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* FoliageQualityComboBox;
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* ShadingQualityComboBox;

  // Advanced settings
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* GlobalIlluminationMethodComboBox;
  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* ReflectionMethodComboBox;
  UPROPERTY(meta = (BindWidget))
  class UCheckBox* DepthOfFieldCheckBox;
  UPROPERTY(meta = (BindWidget))
  class UCheckBox* BloomCheckBox;

  // DLSS FG settings
  UPROPERTY(meta = (BindWidget))
  class UCheckBox* DLSSFrameGenerationCheckBox;

  // Resolution scaling
  UPROPERTY(meta = (BindWidget))
  class USlider* ResolutionScaleSlider;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ResolutionScaleText;

  UPROPERTY(meta = (BindWidget))
  class UButton* ApplyButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* BackButton;

  UPROPERTY(EditAnywhere)
  class ASettingsManager* SettingsManager;

  UFUNCTION()
  void Apply();
  UFUNCTION()
  void Back();
  UFUNCTION()
  void OnOverallQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
  UFUNCTION()
  void OnResolutionScaleChanged(float Value);
  UFUNCTION()
  void OnGlobalIlluminationMethodChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
  UFUNCTION()
  void OnReflectionMethodChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

  void RefreshUI();
  void InitUI(FInUIInputActions _InUIInputActions,
              class ASettingsManager* _SettingsManager,
              std::function<void()> _BackFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  std::function<void()> BackFunc;
};