// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "SoundSettingsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API USoundSettingsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class USlider* MasterVolumeSlider;
  UPROPERTY(meta = (BindWidget))
  class USlider* MusicVolumeSlider;
  UPROPERTY(meta = (BindWidget))
  class USlider* SFXVolumeSlider;

  UPROPERTY(meta = (BindWidget))
  class UButton* ApplyButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* BackButton;

  class ASettingsManager* SettingsManager;

  UFUNCTION()
  void Apply();
  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInUIInputActions _InUIInputActions,
              class ASettingsManager* _SettingsManager,
              std::function<void()> _BackFunc);

  std::function<void()> BackFunc;
};