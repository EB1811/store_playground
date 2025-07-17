
#include "SettingsManager.h"
#include "Misc/AssertionMacros.h"
#include "Sound/SoundClass.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SettingsSaveGame.h"
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

void ASettingsManager::SaveSettings() const { SaveManager->SaveSettingsToDisk(SoundSettings); }
void ASettingsManager::LoadSettings() {
  const USettingsSaveGame* SavedSettings = SaveManager->LoadSettingsFromDisk();

  auto SavedSound = SavedSettings->SoundSettings;
  SetMasterVolume(SavedSound.MasterVolume);
  SetMusicVolume(SavedSound.MusicVolume);
  SetSFXVolume(SavedSound.SFXVolume);
}