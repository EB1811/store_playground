
#include "SettingsManager.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Misc/AssertionMacros.h"
#include "Sound/SoundClass.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SettingsSaveGame.h"
#include "GameFramework/GameUserSettings.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ASettingsManager::ASettingsManager() {
  PrimaryActorTick.bCanEverTick = false;

  SoundSettings = {.MasterVolume = 0.5f, .MusicVolume = 0.5f, .SFXVolume = 0.5f};
}

void ASettingsManager::BeginPlay() {
  Super::BeginPlay();

  check(MasterSoundMix && MasterSoundClass && MusicSoundClass && SFXSoundClass);
}

void ASettingsManager::SetMasterVolume(float Volume) {
  SoundSettings.MasterVolume = Volume;
  UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix, MasterSoundClass, Volume);
}
void ASettingsManager::SetMusicVolume(float Volume) {
  SoundSettings.MusicVolume = Volume;
  UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix, MusicSoundClass, Volume);
}
void ASettingsManager::SetSFXVolume(float Volume) {
  SoundSettings.SFXVolume = Volume;
  UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix, SFXSoundClass, Volume);
}

void ASettingsManager::SetAntiAliasingMethod(int32 Method) {
  static IConsoleVariable* SetAA = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
  check(SetAA);
  SetAA->Set((uint8)Method, ECVF_SetByGameSetting);
  UWorld* world = GEngine->GameViewport->GetWorld();
  if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
    PC->ConsoleCommand("r.AntiAliasingMethod " + FString::FromInt(Method), true);
}

void ASettingsManager::SetGlobalIlluminationMethod(int32 Method) {
  UE_LOG(LogTemp, Log, TEXT("Setting Global Illumination Method to: %d"), Method);
  static IConsoleVariable* SetGI =
      IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicGlobalIlluminationMethod"));
  check(SetGI);

  UWorld* world = GEngine->GameViewport->GetWorld();
  switch (Method) {
    case 0:  // None
      SetGI->Set(0, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
        PC->ConsoleCommand("r.DynamicGlobalIlluminationMethod 0", true);
      break;
    case 1:  // Lumen
      SetGI->Set(1, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
        PC->ConsoleCommand("r.DynamicGlobalIlluminationMethod 1", true);
      break;
    case 2:  // Screen Space
      SetGI->Set(2, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
        PC->ConsoleCommand("r.DynamicGlobalIlluminationMethod 2", true);
      break;
  }
}

void ASettingsManager::SetReflectionMethod(int32 Method) {
  static IConsoleVariable* SetRF = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ReflectionMethod"));
  check(SetRF);

  UWorld* world = GEngine->GameViewport->GetWorld();
  switch (Method) {
    case 0:  // None
      SetRF->Set(0, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
        PC->ConsoleCommand("r.ReflectionMethod 0", true);
      break;
    case 1:  // Lumen
      SetRF->Set(1, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
        PC->ConsoleCommand("r.ReflectionMethod 1", true);
      break;
    case 2:  // Screen Space
      SetRF->Set(2, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
        PC->ConsoleCommand("r.ReflectionMethod 2", true);
      break;
  }
}

void ASettingsManager::SetMotionBlurEnabled(bool bEnabled) {
  static IConsoleVariable* SetMotionBlur = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlur.Amount"));
  check(SetMotionBlur);
  SetMotionBlur->Set(bEnabled ? 1.0f : 0.0f, ECVF_SetByGameSetting);
  UWorld* world = GEngine->GameViewport->GetWorld();
  if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
    PC->ConsoleCommand(FString::Printf(TEXT("r.MotionBlur.Amount %d"), bEnabled ? 1 : 0), true);
}

void ASettingsManager::SetBloomEnabled(bool bEnabled) {
  static IConsoleVariable* SetBloom = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BloomQuality"));
  check(SetBloom);
  SetBloom->Set(bEnabled ? 5 : 0, ECVF_SetByGameSetting);
  UWorld* world = GEngine->GameViewport->GetWorld();
  if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
    PC->ConsoleCommand(FString::Printf(TEXT("r.BloomQuality %d"), bEnabled ? 5 : 0), true);
}

void ASettingsManager::SaveSettings() const {
  UnrealSettings->ApplySettings(true);
  SaveManager->SaveSettingsToDisk(SoundSettings);
}
void ASettingsManager::LoadSettings() {
  if (!UnrealSettings) UnrealSettings = GEngine->GetGameUserSettings();
  check(UnrealSettings);
  UnrealSettings->LoadSettings(true);

  const USettingsSaveGame* SavedSettings = SaveManager->LoadSettingsFromDisk();
  auto SavedSound = SavedSettings->SoundSettings;
  SetMasterVolume(SavedSound.MasterVolume);
  SetMusicVolume(SavedSound.MusicVolume);
  SetSFXVolume(SavedSound.SFXVolume);
}