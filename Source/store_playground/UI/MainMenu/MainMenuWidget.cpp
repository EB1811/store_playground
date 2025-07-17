#include "MainMenuWidget.h"
#include "store_playground/MainMenuControl/MainMenuGameMode.h"
#include "store_playground/Framework/StorePGGameMode.h"
#include "store_playground/UI/SaveSlots/SaveLoadSlotsWidget.h"
#include "store_playground/UI/Settings/SettingsWidget.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ContinueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::Continue);
  NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::NewGame);
  LoadMenuButton->OnClicked.AddDynamic(this, &UMainMenuWidget::LoadMenu);
  SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::SettingsMenu);
  ExitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::Exit);

  SetupUIActionable();
}

void UMainMenuWidget::Continue() {
  if (SaveManager->SaveSlotListSaveGame->MostRecentSaveSlotIndex == -1 ||
      SaveManager->SaveSlotListSaveGame->SaveSlotList.Num() <= 0)
    return;

  AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  check(MainMenuGameMode);

  MainMenuGameMode->Continue();
}
void UMainMenuWidget::NewGame() {
  AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  check(MainMenuGameMode);

  MainMenuGameMode->StartNewGame();
}

void UMainMenuWidget::LoadMenu() {
  auto BackFunc = [this]() { SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed); };
  SaveLoadSlotsWidget->InitUI(InUIInputActions, false, SaveManager, BackFunc);
  SaveLoadSlotsWidget->RefreshUI();

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Visible);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}
void UMainMenuWidget::SettingsMenu() {
  auto BackFunc = [this]() { SettingsWidget->SetVisibility(ESlateVisibility::Collapsed); };
  SettingsWidget->InitUI(InUIInputActions, SettingsManager, BackFunc);

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenuWidget::Exit() {
  AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  check(MainMenuGameMode);

  MainMenuGameMode->Exit();
}

void UMainMenuWidget::RefreshUI() {
  if (SaveManager->SaveSlotListSaveGame->MostRecentSaveSlotIndex == -1 ||
      SaveManager->SaveSlotListSaveGame->SaveSlotList.Num() <= 0)
    ContinueButton->SetIsEnabled(false);
  else ContinueButton->SetIsEnabled(true);
}

void UMainMenuWidget::InitUI(FInUIInputActions _InUIInputActions,
                             class ASettingsManager* _SettingsManager,
                             class ASaveManager* _SaveManager) {
  check(_SettingsManager && _SaveManager);

  InUIInputActions = _InUIInputActions;
  SettingsManager = _SettingsManager;
  SaveManager = _SaveManager;

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UMainMenuWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() {
    if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.AdvanceUI();
  };
  UIActionable.DirectionalInput = [this](FVector2D Direction) {
    if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.DirectionalInput(Direction);
  };
  UIActionable.SideButton4 = [this]() {
    if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.SideButton4;
  };
  UIActionable.RetractUI = [this]() {
    if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.RetractUI();
  };
  UIActionable.QuitUI = [this]() {
    if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.RetractUI();
  };
}