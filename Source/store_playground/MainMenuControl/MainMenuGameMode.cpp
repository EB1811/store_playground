// Fill out your copyright notice in the Description page of Project Settings.

#include "store_playground/MainMenuControl/MainMenuGameMode.h"
#include "Engine/LocalPlayer.h"
#include "Logging/LogVerbosity.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "store_playground/Player/MainMenuPlayer.h"
#include "store_playground/Framework/SettingsManager.h"
#include "store_playground/Framework/StorePGGameInstance.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "store_playground/MainMenuControl/MainMenuControlHUD.h"
#include "store_playground/Sound/MusicManager.h"

void AMainMenuGameMode::BeginPlay() {
  Super::BeginPlay();

  AMainMenuPlayer* MainMenuPlayer = Cast<AMainMenuPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

  MainMenuControlHUD = Cast<AMainMenuControlHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
  check(MainMenuControlHUD && SettingsManagerClass && SaveManagerClass);

  ASettingsManager* SettingsManager = GetWorld()->SpawnActor<ASettingsManager>(SettingsManagerClass);
  SaveManager = GetWorld()->SpawnActor<ASaveManager>(SaveManagerClass);
  MusicManager = GetWorld()->SpawnActor<AMusicManager>(MusicManagerClass);

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
  SaveManager->LoadSaveGameSlots();

  if (!UGameplayStatics::DoesSaveGameExist(SaveManager->SaveManagerParams.SettingsSaveName, 0))
    SettingsManager->InitSettings();
  else SettingsManager->LoadSettings();

  MainMenuControlHUD->InUIInputActions = MainMenuPlayer->InUIInputActions;

  // https://forums.unrealengine.com/t/what-is-the-event-to-check-when-all-materials-and-textures-are-fully-resolved-or-finished-loading-on-a-new-spawned-actor/1166525/9
  FStreamingManagerCollection& SMC = FStreamingManagerCollection::Get();
  SMC.BlockTillAllRequestsFinished(0.f, true);

  if (ULevelStreaming* Streaming = UGameplayStatics::GetStreamingLevel(this, InitLevelName)) {
    FWorldDelegates::LevelAddedToWorld.RemoveAll(this);
    FWorldDelegates::LevelAddedToWorld.AddUObject(this, &AMainMenuGameMode::OnLevelAddedToWorld);

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.UUID = 1;
    UGameplayStatics::LoadStreamLevel(this, InitLevelName, true, true, LatentInfo);
  }
}

void AMainMenuGameMode::OnLevelAddedToWorld(ULevel* InLevel, UWorld* InWorld) {
  check(InWorld == GetWorld());

  UE_LOG(LogTemp, Warning, TEXT("MainMenuGameMode: Level added to world: %s"), *InLevel->GetOuter()->GetName());

  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  FWorldDelegates::LevelAddedToWorld.RemoveAll(this);
  GetWorld()->GetTimerManager().SetTimer(CheckLoadCompleteTimerHandle, this, &AMainMenuGameMode::CheckLoadComplete,
                                         StorePGGameInstance->bFromGame ? 0.5 : 2.0, false);
}
void AMainMenuGameMode::CheckLoadComplete() {
  CheckCount++;
  bool bAllLoaded = true;

  // * Trying a bunch of things to ensure everything is loaded before proceeding.

  // https://forums.unrealengine.com/t/what-is-the-event-to-check-when-all-materials-and-textures-are-fully-resolved-or-finished-loading-on-a-new-spawned-actor/1166525/9
  FStreamingManagerCollection& SMC = FStreamingManagerCollection::Get();
  SMC.BlockTillAllRequestsFinished(0.f, true);

  TArray<AActor*> FoundActors;
  UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AActor::StaticClass(), FName("initlevel_load"), FoundActors);
  if (FoundActors.Num() < 3) bAllLoaded = false;
  for (AActor* Actor : FoundActors) {
    if (Actor->HasAnyFlags(RF_NeedLoad) || Actor->HasAnyFlags(RF_NeedPostLoad)) {
      bAllLoaded = false;
      break;
    } else {
      TArray<UActorComponent*> Components = Actor->GetComponents().Array();
      for (UActorComponent* Component : Components) {
        if (Component->HasAnyFlags(RF_NeedLoad) || Component->HasAnyFlags(RF_NeedPostLoad)) {
          bAllLoaded = false;
          break;
        }
      }
    }
    TArray<UStaticMeshComponent*> StaticMeshComponents;
    Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);
    for (UStaticMeshComponent* SMCom : StaticMeshComponents) {
      TArray<UMaterialInterface*> MaterialInstances;
      SMCom->GetUsedMaterials(MaterialInstances);
      if (MaterialInstances.Num() == 0) {
        bAllLoaded = false;
        break;
      }
      for (UMaterialInterface* MI : MaterialInstances) {
        if (MI->HasAnyFlags(RF_NeedLoad) || MI->HasAnyFlags(RF_NeedPostLoad)) {
          bAllLoaded = false;
          break;
        }
      }
    }
  }

  if (!bAllLoaded && CheckCount < 9) {
    UE_LOG(LogTemp, Warning, TEXT("MainMenuGameMode: Load not complete, rechecking..."));
    GetWorld()->GetTimerManager().SetTimer(CheckLoadCompleteTimerHandle, this, &AMainMenuGameMode::CheckLoadComplete,
                                           1.0, false);
    return;
  }
  UE_LOG(LogTemp, Warning, TEXT("MainMenuGameMode: Load complete checks done."));

  MainMenuControlHUD->OpenMainMenu();

  MusicManager->MainMenuMusicCalled();
}

void AMainMenuGameMode::Continue() {
  if (SaveManager->SaveSlotListSaveGame->MostRecentSaveSlotIndex == 0 &&
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