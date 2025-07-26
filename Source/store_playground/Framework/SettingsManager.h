// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "SettingsManager.generated.h"

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ASettingsManager : public AInfo {
  GENERATED_BODY()

public:
  ASettingsManager();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere)
  class UGameUserSettings* UnrealSettings;
  UPROPERTY(EditAnywhere)
  class ASaveManager* SaveManager;

  UPROPERTY(EditAnywhere, Category = "Sound")
  FSavedSoundSettings SoundSettings;
  UPROPERTY(EditAnywhere, Category = "Sound")
  class USoundMix* MasterSoundMix;
  UPROPERTY(EditAnywhere, Category = "Sound")
  class USoundClass* MasterSoundClass;
  UPROPERTY(EditAnywhere, Category = "Sound")
  class USoundClass* MusicSoundClass;
  UPROPERTY(EditAnywhere, Category = "Sound")
  class USoundClass* SFXSoundClass;

  void SetMasterVolume(float Volume);
  void SetMusicVolume(float Volume);
  void SetSFXVolume(float Volume);

  void SetAntiAliasingMethod(int32 Method);
  void SetGlobalIlluminationMethod(int32 Method);
  void SetReflectionMethod(int32 Method);
  void SetMotionBlurEnabled(bool bEnabled);
  void SetBloomEnabled(bool bEnabled);

  void SaveSettings() const;
  void LoadSettings();
};