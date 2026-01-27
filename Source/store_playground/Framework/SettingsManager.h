// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "SettingsStructs.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "SettingsManager.generated.h"

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ASettingsManager : public AInfo {
  GENERATED_BODY()

public:
  ASettingsManager();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere)
  class ASaveManager* SaveManager;
  // * For changing on difficulty change.
  UPROPERTY(EditAnywhere)
  class ADayManager* DayManager;

  UPROPERTY(EditAnywhere)
  class UGameUserSettings* UnrealSettings;
  UPROPERTY(EditAnywhere)
  class UEnhancedInputUserSettings* EInputUserSettings;

  UPROPERTY(EditAnywhere)
  FGameSettings GameSettings;

  UPROPERTY(EditAnywhere)
  FSavedSoundSettings SoundSettings;
  UPROPERTY(EditAnywhere)
  class USoundMix* MasterSoundMix;
  UPROPERTY(EditAnywhere)
  class USoundClass* MasterSoundClass;
  UPROPERTY(EditAnywhere)
  class USoundClass* MusicSoundClass;
  UPROPERTY(EditAnywhere)
  class USoundClass* SFXSoundClass;

  UPROPERTY(EditAnywhere)
  FAdvGraphicsSettings AdvGraphicsSettings;

  void SetGameSettings(const FGameSettings& NewSettings);

  void SetMasterVolume(float Volume);
  void SetMusicVolume(float Volume);
  void SetSFXVolume(float Volume);

  void SetRenderMethod(int32 Method);  // * Needs restart.
  void SetAntiAliasingMethod(int32 Method);
  void SetGlobalIlluminationMethod(int32 Method);
  void SetReflectionMethod(int32 Method);
  void SetDepthOfFieldEnabled(bool bEnabled);
  void SetBloomEnabled(bool bEnabled);
  void SetDLSSFrameGenerationEnabled(bool bEnabled);
  void SetFastGrassSpawning(bool bEnabled);

  void SetScaleAdvGraphicsSettings();  // * Based on scalability settings.

  void SaveSettings() const;
  void LoadSettings();

  void InitSettings();  // * Init graphics settings based on benchmark.

  UFUNCTION(BlueprintNativeEvent)
  bool IsDlssAvailable();
  UFUNCTION(BlueprintNativeEvent)
  bool IsFrameGenAvailable();
};