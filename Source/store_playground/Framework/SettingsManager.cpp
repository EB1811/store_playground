
#include "SettingsManager.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "EnhancedInputSubsystems.h"
#include "Logging/LogVerbosity.h"
#include "Misc/AssertionMacros.h"
#include "LandscapeSubsystem.h"
#include "Misc/ConfigCacheIni.h"
#include "Sound/SoundClass.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SettingsSaveGame.h"
#include "store_playground/DayManager/DayManager.h"
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

void ASettingsManager::SetGameSettings(const FGameSettings& NewSettings) {
  GameSettings = NewSettings;

  // * If game already running, apply settings immediately.
  if (DayManager) DayManager->RecalculateNextDebt();
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

void ASettingsManager::SetRenderMethod(int32 Method) {
  UE_LOG(LogTemp, Log, TEXT("Setting Render Method to: %d"), Method);
  // 0=Default, 1=DirectX12, 2=Vulkan
  if (Method == 0) {
    GConfig->SetString(TEXT("/Script/WindowsTargetPlatform.WindowsTargetSettings"), TEXT("DefaultGraphicsRHI"),
                       TEXT("DefaultGraphicsRHI_Default"), GEngineIni);
  } else if (Method == 1) {
    GConfig->SetString(TEXT("/Script/WindowsTargetPlatform.WindowsTargetSettings"), TEXT("DefaultGraphicsRHI"),
                       TEXT("DefaultGraphicsRHI_DX12"), GEngineIni);
  } else if (Method == 2) {
    GConfig->SetString(TEXT("/Script/WindowsTargetPlatform.WindowsTargetSettings"), TEXT("DefaultGraphicsRHI"),
                       TEXT("DefaultGraphicsRHI_Vulkan"), GEngineIni);
  } else {
    checkNoEntry();
  }
  GConfig->Flush(true, GEngineIni);

  AdvGraphicsSettings.RenderMethod = Method;
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
    UWorld* World = GEngine->GameViewport->GetWorld();
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0)) {
      PC->ConsoleCommand("r.AntiAliasingMethod 4", true);
      PC->ConsoleCommand("r.NGX.DLSS.Enable 1", true);
    }
    AdvGraphicsSettings.AntiAliasingMethod = Method;
    return;
  } else {
    static IConsoleVariable* SetDLSS = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.Enable"));
    if (!SetDLSS) return;

    SetDLSS->Set(0, ECVF_SetByGameSetting);
    UWorld* World = GEngine->GameViewport->GetWorld();
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
      PC->ConsoleCommand("r.NGX.DLSS.Enable 0", true);
  }

  static IConsoleVariable* SetAA = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
  check(SetAA);
  SetAA->Set((uint8)Method, ECVF_SetByGameSetting);
  UWorld* World = GEngine->GameViewport->GetWorld();
  if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
    PC->ConsoleCommand("r.AntiAliasingMethod " + FString::FromInt(Method), true);
  AdvGraphicsSettings.AntiAliasingMethod = Method;
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

  UWorld* World = GEngine->GameViewport->GetWorld();
  switch (Method) {
    case 0:  // None
      SetGI->Set(0, ECVF_SetByGameSetting);
      SetDiffuseGI->Set(0, ECVF_SetByGameSetting);
      SetNaniteGI->Set(0, ECVF_SetByGameSetting);
      SetVSM->Set(0, ECVF_SetByGameSetting);
      SetAllRT->Set(0, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0)) {
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
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0)) {
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
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0)) {
        PC->ConsoleCommand("r.DynamicGlobalIlluminationMethod 2", true);
        PC->ConsoleCommand("r.Lumen.DiffuseIndirect.Allow 0", true);
        PC->ConsoleCommand("r.Nanite 0", true);
        PC->ConsoleCommand("r.Shadow.Virtual.Enable 0", true);
        PC->ConsoleCommand("r.RayTracing.ForceAllRayTracingEffects 0", true);
      }
      break;
  }
  AdvGraphicsSettings.GlobalIlluminationMethod = Method;
}
void ASettingsManager::SetReflectionMethod(int32 Method) {
  static IConsoleVariable* SetRF = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ReflectionMethod"));
  check(SetRF);

  UWorld* World = GEngine->GameViewport->GetWorld();
  switch (Method) {
    case 0:  // None
      SetRF->Set(0, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
        PC->ConsoleCommand("r.ReflectionMethod 0", true);
      break;
    case 1:  // Lumen
      SetRF->Set(1, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
        PC->ConsoleCommand("r.ReflectionMethod 1", true);
      break;
    case 2:  // Screen Space
      SetRF->Set(2, ECVF_SetByGameSetting);
      if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
        PC->ConsoleCommand("r.ReflectionMethod 2", true);
      break;
  }
  AdvGraphicsSettings.ReflectionMethod = Method;
}

void ASettingsManager::SetDepthOfFieldEnabled(bool bEnabled) {
  static IConsoleVariable* SetDoF = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DepthOfFieldQuality"));
  check(SetDoF);
  SetDoF->Set(bEnabled ? 2 : 0, ECVF_SetByGameSetting);
  UWorld* World = GEngine->GameViewport->GetWorld();
  if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
    PC->ConsoleCommand(FString::Printf(TEXT("r.DepthOfFieldQuality %d"), bEnabled ? 2 : 0), true);
  AdvGraphicsSettings.bDepthOfField = bEnabled;
}
void ASettingsManager::SetBloomEnabled(bool bEnabled) {
  static IConsoleVariable* SetBloom = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BloomQuality"));
  check(SetBloom);
  SetBloom->Set(bEnabled ? 5 : 0, ECVF_SetByGameSetting);
  UWorld* World = GEngine->GameViewport->GetWorld();
  if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
    PC->ConsoleCommand(FString::Printf(TEXT("r.BloomQuality %d"), bEnabled ? 5 : 0), true);
  AdvGraphicsSettings.bBloom = bEnabled;
}
void ASettingsManager::SetDLSSFrameGenerationEnabled(bool bEnabled) {
  static IConsoleVariable* SetDLSSG = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streamline.DLSSG.Enable"));
  if (!SetDLSSG) return;

  SetDLSSG->Set(bEnabled ? 2 : 0, ECVF_SetByGameSetting);
  UWorld* World = GEngine->GameViewport->GetWorld();
  if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
    PC->ConsoleCommand(FString::Printf(TEXT("r.Streamline.DLSSG.Enable %d"), bEnabled ? 2 : 0), true);
  AdvGraphicsSettings.bDLSSFrameGeneration = bEnabled;
}

void ASettingsManager::SetFastGrassSpawning(bool bEnabled) {
  static IConsoleVariable* SetMaxAsyncTasks = IConsoleManager::Get().FindConsoleVariable(TEXT("grass.MaxAsyncTasks"));
  static IConsoleVariable* SetMaxCreatePerFrame =
      IConsoleManager::Get().FindConsoleVariable(TEXT("grass.MaxCreatePerFrame"));
  check(SetMaxAsyncTasks && SetMaxCreatePerFrame);

  // Prefer aggressive spawning.
  SetMaxAsyncTasks->Set(bEnabled ? 10 : 5, ECVF_SetByGameSetting);
  SetMaxCreatePerFrame->Set(bEnabled ? 5 : 2, ECVF_SetByGameSetting);
  UWorld* World = GEngine->GameViewport->GetWorld();
  if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0)) {
    PC->ConsoleCommand(FString::Printf(TEXT("grass.MaxAsyncTasks %d"), bEnabled ? 10 : 5), true);
    PC->ConsoleCommand(FString::Printf(TEXT("grass.MaxCreatePerFrame %d"), bEnabled ? 5 : 2), true);
  }

  // ? Not sure if this makes the previous settings useless.
  ULandscapeSubsystem* LandscapeSubsystem = World->GetSubsystem<ULandscapeSubsystem>();
  check(LandscapeSubsystem);
  if (bEnabled) LandscapeSubsystem->PrioritizeGrassCreation(true);
  else LandscapeSubsystem->PrioritizeGrassCreation(false);
}

void ASettingsManager::SetScaleAdvGraphicsSettings() {
  int32 FoliageQuality = UnrealSettings->GetFoliageQuality();
  if (FoliageQuality >= 2)  // High or Epic
    SetFastGrassSpawning(true);
  else SetFastGrassSpawning(false);
}

void ASettingsManager::SaveSettings() const {
  UnrealSettings->ApplySettings(true);
  EInputUserSettings->SaveSettings();
  SaveManager->SaveSettingsToDisk(GameSettings, SoundSettings, AdvGraphicsSettings);
}
void ASettingsManager::LoadSettings() {
  if (!UnrealSettings) UnrealSettings = GEngine->GetGameUserSettings();
  check(UnrealSettings);
  UnrealSettings->LoadSettings(true);

  if (!EInputUserSettings) {
    UEnhancedInputLocalPlayerSubsystem* EISubsystem =
        GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    EInputUserSettings = EISubsystem->GetUserSettings();
  }

  const USettingsSaveGame* SavedSettings = SaveManager->LoadSettingsFromDisk();

  auto& SavedGameSettings = SavedSettings->GameSettings;
  SetGameSettings(SavedGameSettings);

  auto& SavedSound = SavedSettings->SoundSettings;
  SetMasterVolume(SavedSound.MasterVolume);
  SetMusicVolume(SavedSound.MusicVolume);
  SetSFXVolume(SavedSound.SFXVolume);

  auto& SavedAdvGraphicsSettings = SavedSettings->AdvGraphicsSettings;
  SetAntiAliasingMethod(SavedAdvGraphicsSettings.AntiAliasingMethod);
  SetGlobalIlluminationMethod(SavedAdvGraphicsSettings.GlobalIlluminationMethod);
  SetReflectionMethod(SavedAdvGraphicsSettings.ReflectionMethod);
  SetDepthOfFieldEnabled(SavedAdvGraphicsSettings.bDepthOfField);
  SetBloomEnabled(SavedAdvGraphicsSettings.bBloom);
  SetDLSSFrameGenerationEnabled(SavedAdvGraphicsSettings.bDLSSFrameGeneration);
  SetScaleAdvGraphicsSettings();

  AdvGraphicsSettings.RenderMethod = SavedAdvGraphicsSettings.RenderMethod;
}

void ASettingsManager::InitSettings() {
  if (!UnrealSettings) UnrealSettings = GEngine->GetGameUserSettings();
  check(UnrealSettings);
  if (!EInputUserSettings) {
    UEnhancedInputLocalPlayerSubsystem* EISubsystem =
        GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    EInputUserSettings = EISubsystem->GetUserSettings();
  }
  check(EInputUserSettings);

  // Running a benchmark to set initial graphics settings.
  UnrealSettings->RunHardwareBenchmark(10, 1.0f, 1.0f);
  UnrealSettings->ApplyHardwareBenchmarkResults();

  int32 CurrentOverallQuality = UnrealSettings->GetOverallScalabilityLevel();
  UE_LOG(LogTemp, Warning, TEXT("Benchmark Overall Quality Level: %d"), CurrentOverallQuality);
  if (CurrentOverallQuality >= 3) {  // Epic
    SetAntiAliasingMethod(0);
    SetGlobalIlluminationMethod(0);
    SetReflectionMethod(2);  // Screen Space
    SetDepthOfFieldEnabled(true);
    SetBloomEnabled(true);
    SetDLSSFrameGenerationEnabled(false);
  } else if (CurrentOverallQuality == 2) {  // High
    SetAntiAliasingMethod(0);
    SetGlobalIlluminationMethod(0);
    SetReflectionMethod(0);
    SetDepthOfFieldEnabled(true);
    SetBloomEnabled(true);
    SetDLSSFrameGenerationEnabled(false);
  } else if (CurrentOverallQuality == 1) {  // Medium
    SetAntiAliasingMethod(0);
    SetGlobalIlluminationMethod(0);
    SetReflectionMethod(0);
    SetDepthOfFieldEnabled(true);
    SetBloomEnabled(true);
    SetDLSSFrameGenerationEnabled(false);
  } else {  // Low
    SetAntiAliasingMethod(0);
    SetGlobalIlluminationMethod(0);
    SetReflectionMethod(0);
    SetDepthOfFieldEnabled(false);
    SetBloomEnabled(false);
    SetDLSSFrameGenerationEnabled(false);
  }
  SetScaleAdvGraphicsSettings();
  AdvGraphicsSettings.RenderMethod = 0;  // Default

  GameSettings = {
      .Difficulty = EGameDifficulty::Normal,
      .bShowTutorials = true,
  };
  SetGameSettings(GameSettings);

  SoundSettings = {.MasterVolume = 0.5f, .MusicVolume = 1.0f, .SFXVolume = 1.0f};
  SetMasterVolume(SoundSettings.MasterVolume);
  SetMusicVolume(SoundSettings.MusicVolume);
  SetSFXVolume(SoundSettings.SFXVolume);

  SaveSettings();
}

auto ASettingsManager::IsDlssAvailable() const -> bool {
  static IConsoleVariable* CVarDLSS = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.Enable"));
  return CVarDLSS != nullptr;
}
auto ASettingsManager::IsFrameGenAvailable() const -> bool {
  static IConsoleVariable* CVarDLSSG = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streamline.DLSSG.Enable"));
  return CVarDLSSG != nullptr;
}