#include "MainMenuControlHUD.h"
#include "Components/Button.h"
#include "Components/SlateWrapperTypes.h"
#include "store_playground/MainMenuControl/SettingsSaveGame.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Kismet/GameplayStatics.h"
#include "store_playground/UI/MainMenu/MainMenuWidget.h"
#include "store_playground/UI/Transitions/LevelLoadingTransitionWidget.h"

AMainMenuControlHUD::AMainMenuControlHUD() { PrimaryActorTick.bCanEverTick = false; }

void AMainMenuControlHUD::DrawHUD() { Super::DrawHUD(); }

void AMainMenuControlHUD::BeginPlay() {
  Super::BeginPlay();

  check(MainMenuWidgetClass && LevelLoadingTransitionWidgetClass);

  MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
  MainMenuWidget->AddToViewport(20);
  MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
  MainMenuWidget->NewGameButton->OnClicked.AddDynamic(this, &AMainMenuControlHUD::StartNewGame);
  MainMenuWidget->ContinueButton->OnClicked.AddDynamic(this, &AMainMenuControlHUD::ContinueGame);

  LevelLoadingTransitionWidget =
      CreateWidget<ULevelLoadingTransitionWidget>(GetWorld(), LevelLoadingTransitionWidgetClass);

  const FInputModeUIOnly InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  LoadSettings();
}

void AMainMenuControlHUD::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AMainMenuControlHUD::CreateNewSaveGame() {
  CurrentSettingsSaveGame =
      Cast<USettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(USettingsSaveGame::StaticClass()));
  check(CurrentSettingsSaveGame);

  CurrentSettingsSaveGame->SlotName = "Settings";
}

void AMainMenuControlHUD::SaveSettings() {
  if (!CurrentSettingsSaveGame) CreateNewSaveGame();

  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  CurrentSettingsSaveGame->GameSettings = StorePGGameInstance->GameSettings;

  UGameplayStatics::SaveGameToSlot(CurrentSettingsSaveGame, CurrentSettingsSaveGame->SlotName, 0);
}
void AMainMenuControlHUD::LoadSettings() {
  if (!UGameplayStatics::DoesSaveGameExist("Settings", 0)) return CreateNewSaveGame();

  CurrentSettingsSaveGame = Cast<USettingsSaveGame>(UGameplayStatics::LoadGameFromSlot("Settings", 0));
  check(CurrentSettingsSaveGame);

  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  StorePGGameInstance->GameSettings = CurrentSettingsSaveGame->GameSettings;
}

void AMainMenuControlHUD::StartNewGame() {
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  StorePGGameInstance->bFromSaveGame = false;

  LevelLoadingTransitionWidget->FadeInEndFunc = [this]() { UGameplayStatics::OpenLevel(GetWorld(), "StartMap", true); };

  LevelLoadingTransitionWidget->AddToViewport(100);
  LevelLoadingTransitionWidget->SetVisibility(ESlateVisibility::Visible);
}

void AMainMenuControlHUD::ContinueGame() {
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  StorePGGameInstance->bFromSaveGame = true;

  LevelLoadingTransitionWidget->FadeInEndFunc = [this]() { UGameplayStatics::OpenLevel(GetWorld(), "StartMap", true); };

  LevelLoadingTransitionWidget->AddToViewport(100);
  LevelLoadingTransitionWidget->SetVisibility(ESlateVisibility::Visible);
}
