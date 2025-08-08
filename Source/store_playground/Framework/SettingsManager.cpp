
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
  // * DLSS
  if (Method == 5) {
    static IConsoleVariable* SetAA = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
    static IConsoleVariable* SetDLSS = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.Enable"));

    check(SetAA);
    if (!SetDLSS) return;

    SetAA->Set(4, ECVF_SetByGameSetting);
    SetDLSS->Set(1, ECVF_SetByGameSetting);
    UWorld* world = GEngine->GameViewport->GetWorld();
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0)) {
      PC->ConsoleCommand("r.AntiAliasingMethod 4", true);
      PC->ConsoleCommand("r.NGX.DLSS.Enable 1", true);
    }
    return;
  } else {
    static IConsoleVariable* SetDLSS = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.Enable"));
    if (!SetDLSS) return;

    SetDLSS->Set(0, ECVF_SetByGameSetting);
    UWorld* world = GEngine->GameViewport->GetWorld();
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
      PC->ConsoleCommand("r.NGX.DLSS.Enable 0", true);
  }

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
  static IConsoleVariable* SetDiffuseGI =
      IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.DiffuseIndirect.Allow"));
  static IConsoleVariable* SetNaniteGI = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Nanite"));
  static IConsoleVariable* SetVSM = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.Virtual.Enable"));
  static IConsoleVariable* SetAllRT =
      IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.ForceAllRayTracingEffects"));
  check(SetGI && SetDiffuseGI && SetNaniteGI && SetVSM && SetAllRT);

  UWorld* world = GEngine->GameViewport->GetWorld();
  switch (Method) {
    case 0:  // None
      SetGI->Set(0, ECVF_SetByGameSetting);
      SetDiffuseGI->Set(0, ECVF_SetByGameSetting);
      SetNaniteGI->Set(0, ECVF_SetByGameSetting);
      SetVSM->Set(0, ECVF_SetByGameSetting);
      SetAllRT->Set(0, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0)) {
        PC->ConsoleCommand("r.DynamicGlobalIlluminationMethod 0", true);
        PC->ConsoleCommand("r.Lumen.DiffuseIndirect.Allow 0", true);
        PC->ConsoleCommand("r.Nanite 0", true);
        PC->ConsoleCommand("r.Shadow.Virtual.Enable 0", true);
        PC->ConsoleCommand("r.RayTracing.ForceAllRayTracingEffects 0", true);
      }
      break;
    case 1:  // Lumen
      SetGI->Set(1, ECVF_SetByGameSetting);
      SetDiffuseGI->Set(1, ECVF_SetByGameSetting);
      SetNaniteGI->Set(1, ECVF_SetByGameSetting);
      SetVSM->Set(1, ECVF_SetByGameSetting);
      SetAllRT->Set(-1, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0)) {
        PC->ConsoleCommand("r.DynamicGlobalIlluminationMethod 1", true);
        PC->ConsoleCommand("r.Lumen.DiffuseIndirect.Allow 1", true);
        PC->ConsoleCommand("r.Nanite 1", true);
        PC->ConsoleCommand("r.Shadow.Virtual.Enable 1", true);
        PC->ConsoleCommand("r.RayTracing.ForceAllRayTracingEffects -1", true);
      }
      break;
    case 2:  // Screen Space
      SetGI->Set(2, ECVF_SetByGameSetting);
      SetNaniteGI->Set(0, ECVF_SetByGameSetting);
      SetDiffuseGI->Set(0, ECVF_SetByGameSetting);
      SetVSM->Set(0, ECVF_SetByGameSetting);
      SetAllRT->Set(0, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0)) {
        PC->ConsoleCommand("r.DynamicGlobalIlluminationMethod 2", true);
        PC->ConsoleCommand("r.Lumen.DiffuseIndirect.Allow 0", true);
        PC->ConsoleCommand("r.Nanite 0", true);
        PC->ConsoleCommand("r.Shadow.Virtual.Enable 0", true);
        PC->ConsoleCommand("r.RayTracing.ForceAllRayTracingEffects 0", true);
      }
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

void ASettingsManager::SetDepthOfFieldEnabled(bool bEnabled) {
  static IConsoleVariable* SetDoF = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DepthOfFieldQuality"));
  check(SetDoF);
  SetDoF->Set(bEnabled ? 2 : 0, ECVF_SetByGameSetting);
  UWorld* world = GEngine->GameViewport->GetWorld();
  if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
    PC->ConsoleCommand(FString::Printf(TEXT("r.DepthOfFieldQuality %d"), bEnabled ? 2 : 0), true);
}

void ASettingsManager::SetBloomEnabled(bool bEnabled) {
  static IConsoleVariable* SetBloom = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BloomQuality"));
  check(SetBloom);
  SetBloom->Set(bEnabled ? 5 : 0, ECVF_SetByGameSetting);
  UWorld* world = GEngine->GameViewport->GetWorld();
  if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
    PC->ConsoleCommand(FString::Printf(TEXT("r.BloomQuality %d"), bEnabled ? 5 : 0), true);
}

void ASettingsManager::SetDLSSFrameGenerationEnabled(bool bEnabled) {
  static IConsoleVariable* SetDLSSG = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streamline.DLSSG.Enable"));
  if (!SetDLSSG) return;

  SetDLSSG->Set(bEnabled ? 2 : 0, ECVF_SetByGameSetting);
  UWorld* world = GEngine->GameViewport->GetWorld();
  if (APlayerController* PC = UGameplayStatics::GetPlayerController(world, 0))
    PC->ConsoleCommand(FString::Printf(TEXT("r.Streamline.DLSSG.Enable %d"), bEnabled ? 2 : 0), true);
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