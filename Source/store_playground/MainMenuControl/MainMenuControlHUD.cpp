#include "MainMenuControlHUD.h"
#include "Components/Button.h"
#include "Components/SlateWrapperTypes.h"
#include "store_playground/MainMenuControl/SettingsSaveGame.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Kismet/GameplayStatics.h"
#include "store_playground/UI/MainMenu/MainMenuWidget.h"
#include "store_playground/UI/SaveSlots/LoadSlotsWidget.h"
#include "store_playground/UI/Transitions/LevelLoadingTransitionWidget.h"

AMainMenuControlHUD::AMainMenuControlHUD() {
  PrimaryActorTick.bCanEverTick = false;

  MainMenuControlParams.SaveSlotListSaveName = "SaveSlotList";
  MainMenuControlParams.SaveSlotCount = 4;
  MainMenuControlParams.SaveSlotNamePrefix = "SaveSlot_";
}

void AMainMenuControlHUD::DrawHUD() { Super::DrawHUD(); }

void AMainMenuControlHUD::BeginPlay() {
  Super::BeginPlay();

  check(MainMenuWidgetClass && LoadSlotsWidgetClass && LevelLoadingTransitionWidgetClass);

  LoadSettings();
  LoadSaveGameSlots();

  MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
  MainMenuWidget->AddToViewport(20);
  MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
  MainMenuWidget->NewGameButton->OnClicked.AddDynamic(this, &AMainMenuControlHUD::StartNewGame);
  MainMenuWidget->ContinueButton->OnClicked.AddDynamic(this, &AMainMenuControlHUD::ContinueGame);
  MainMenuWidget->LoadGameButton->OnClicked.AddDynamic(this, &AMainMenuControlHUD::LoadGame);

  MainMenuWidget->ContinueButton->SetIsEnabled(SaveSlotListSaveGame->SaveSlotList.Num() > 0);

  LoadSlotsWidget = CreateWidget<ULoadSlotsWidget>(GetWorld(), LoadSlotsWidgetClass);
  LoadSlotsWidget->AddToViewport(20);
  LoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  LoadSlotsWidget->ExitLoadGameMenuButton->OnClicked.AddDynamic(this, &AMainMenuControlHUD::ExitLoadGameMenu);

  LevelLoadingTransitionWidget =
      CreateWidget<ULevelLoadingTransitionWidget>(GetWorld(), LevelLoadingTransitionWidgetClass);

  const FInputModeUIOnly InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);
}

void AMainMenuControlHUD::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AMainMenuControlHUD::StartNewGame() {
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  StorePGGameInstance->bFromSaveGame = false;

  LevelLoadingTransitionWidget->FadeInEndFunc = [this]() { UGameplayStatics::OpenLevel(GetWorld(), "StartMap", true); };

  LevelLoadingTransitionWidget->AddToViewport(100);
  LevelLoadingTransitionWidget->SetVisibility(ESlateVisibility::Visible);
}

void AMainMenuControlHUD::ContinueGame() {
  if (SaveSlotListSaveGame->MostRecentSaveSlotIndex == -1 || SaveSlotListSaveGame->SaveSlotList.Num() <= 0) return;

  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  StorePGGameInstance->bFromSaveGame = true;
  StorePGGameInstance->SaveSlotIndex = SaveSlotListSaveGame->MostRecentSaveSlotIndex;

  LevelLoadingTransitionWidget->FadeInEndFunc = [this]() { UGameplayStatics::OpenLevel(GetWorld(), "StartMap", true); };

  LevelLoadingTransitionWidget->AddToViewport(100);
  LevelLoadingTransitionWidget->SetVisibility(ESlateVisibility::Visible);
}

void AMainMenuControlHUD::LoadGame() {
  MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
  LoadSlotsWidget->SetVisibility(ESlateVisibility::Visible);

  LoadSlotsWidget->SaveSlotListSaveGame = SaveSlotListSaveGame;
  LoadSlotsWidget->SlotCount = MainMenuControlParams.SaveSlotCount;
  LoadSlotsWidget->SelectSlotFunc = [this](int32 SlotIndex) { LoadFromSaveSlot(SlotIndex); };

  LoadSlotsWidget->RefreshUI();
}

void AMainMenuControlHUD::ExitLoadGameMenu() {
  MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
  LoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void AMainMenuControlHUD::CreateNewSettingsSaveGame() {
  CurrentSettingsSaveGame =
      Cast<USettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(USettingsSaveGame::StaticClass()));
  check(CurrentSettingsSaveGame);

  CurrentSettingsSaveGame->SlotName = "Settings";
}

void AMainMenuControlHUD::SaveSettings() {
  if (!CurrentSettingsSaveGame) CreateNewSettingsSaveGame();

  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  CurrentSettingsSaveGame->GameSettings = StorePGGameInstance->GameSettings;

  UGameplayStatics::SaveGameToSlot(CurrentSettingsSaveGame, CurrentSettingsSaveGame->SlotName, 0);
}
void AMainMenuControlHUD::LoadSettings() {
  if (!UGameplayStatics::DoesSaveGameExist("Settings", 0)) return CreateNewSettingsSaveGame();

  CurrentSettingsSaveGame = Cast<USettingsSaveGame>(UGameplayStatics::LoadGameFromSlot("Settings", 0));
  check(CurrentSettingsSaveGame);

  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  StorePGGameInstance->GameSettings = CurrentSettingsSaveGame->GameSettings;
}

void AMainMenuControlHUD::LoadSaveGameSlots() {
  if (!UGameplayStatics::DoesSaveGameExist(MainMenuControlParams.SaveSlotListSaveName, 0)) {
    SaveSlotListSaveGame =
        Cast<USaveSlotListSaveGame>(UGameplayStatics::CreateSaveGameObject(USaveSlotListSaveGame::StaticClass()));
    check(SaveSlotListSaveGame);

    SaveSlotListSaveGame->SaveSlotList = {};
    SaveSlotListSaveGame->MostRecentSaveSlotIndex = -1;

    UGameplayStatics::SaveGameToSlot(SaveSlotListSaveGame, MainMenuControlParams.SaveSlotListSaveName, 0);
    UE_LOG(LogTemp, Warning, TEXT("Created new save slot list."));

    return;
  }

  SaveSlotListSaveGame =
      Cast<USaveSlotListSaveGame>(UGameplayStatics::LoadGameFromSlot(MainMenuControlParams.SaveSlotListSaveName, 0));
  check(SaveSlotListSaveGame);
}

void AMainMenuControlHUD::LoadFromSaveSlot(int32 SlotIndex) {
  check(SaveSlotListSaveGame && SlotIndex < MainMenuControlParams.SaveSlotCount);

  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  StorePGGameInstance->bFromSaveGame = true;
  StorePGGameInstance->SaveSlotIndex = SlotIndex;

  LevelLoadingTransitionWidget->FadeInEndFunc = [this]() { UGameplayStatics::OpenLevel(GetWorld(), "StartMap", true); };

  LevelLoadingTransitionWidget->AddToViewport(100);
  LevelLoadingTransitionWidget->SetVisibility(ESlateVisibility::Visible);
}