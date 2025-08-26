// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/MainMenuControl/MainMenuGameMode.h"
#include "Engine/LocalPlayer.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "store_playground/Player/MainMenuPlayer.h"
#include "store_playground/Framework/SettingsManager.h"
#include "store_playground/Framework/StorePGGameInstance.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "store_playground/MainMenuControl/MainMenuControlHUD.h"

void AMainMenuGameMode::BeginPlay() {
  Super::BeginPlay();

  AMainMenuPlayer* MainMenuPlayer = Cast<AMainMenuPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

  MainMenuControlHUD = Cast<AMainMenuControlHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
  check(MainMenuControlHUD && SettingsManagerClass && SaveManagerClass);

  ASettingsManager* SettingsManager = GetWorld()->SpawnActor<ASettingsManager>(SettingsManagerClass);
  SaveManager = GetWorld()->SpawnActor<ASaveManager>(SaveManagerClass);

  MainMenuControlHUD->SettingsManager = SettingsManager;
  MainMenuControlHUD->SaveManager = SaveManager;

  SettingsManager->SaveManager = SaveManager;

  // * Settings and save data.
  UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
      GetWorld()->GetFirstPlayerController()->GetLocalPlayer());
  check(EISubsystem);
  UEnhancedInputUserSettings* EISettings = EISubsystem->GetUserSettings();
  check(EISettings);
  for (const auto& StateContext : MainMenuPlayer->InputContexts)
    EISettings->RegisterInputMappingContext(StateContext.Value);
  SettingsManager->LoadSettings();
  SaveManager->LoadSaveGameSlots();

  MainMenuControlHUD->OpenMainMenu();
}

void AMainMenuGameMode::Continue() {
  if (SaveManager->SaveSlotListSaveGame->MostRecentSaveSlotIndex == -1 ||
      SaveManager->SaveSlotListSaveGame->SaveSlotList.Num() <= 0)
    return;

  int32 SlotIndex = SaveManager->SaveSlotListSaveGame->MostRecentSaveSlotIndex;

  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);
  StorePGGameInstance->bFromSaveGame = true;
  StorePGGameInstance->SaveSlotIndex = SlotIndex;
  StorePGGameInstance->bFromGameOver = false;

  MainMenuControlHUD->StartLevelLoadingTransition(
      [this]() { UGameplayStatics::OpenLevel(GetWorld(), "StartMap", true); });
}
void AMainMenuGameMode::StartNewGame() {
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);

  StorePGGameInstance->bFromSaveGame = false;
  StorePGGameInstance->bFromGameOver = false;

  MainMenuControlHUD->StartLevelLoadingTransition(
      [this]() { UGameplayStatics::OpenLevel(GetWorld(), "StartMap", true); });
}
void AMainMenuGameMode::LoadGame(int32 SlotIndex) {
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);

  StorePGGameInstance->bFromSaveGame = true;
  StorePGGameInstance->SaveSlotIndex = SlotIndex;
  StorePGGameInstance->bFromGameOver = false;

  MainMenuControlHUD->StartLevelLoadingTransition(
      [this]() { UGameplayStatics::OpenLevel(GetWorld(), "StartMap", true); });
}

void AMainMenuGameMode::Exit() { UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true); }