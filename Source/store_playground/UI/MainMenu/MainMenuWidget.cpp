#include "MainMenuWidget.h"
#include "store_playground/MainMenuControl/MainMenuGameMode.h"
#include "store_playground/Framework/StorePGGameMode.h"
#include "store_playground/UI/MainMenu/NewGameSetupWidget.h"
#include "store_playground/UI/SaveSlots/SaveLoadSlotsWidget.h"
#include "store_playground/UI/Settings/SettingsWidget.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ContinueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::Continue);
  ContinueButton->OnHovered.AddDynamic(this, &UMainMenuWidget::UnhoverButton);
  NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::NewGame);
  NewGameButton->OnHovered.AddDynamic(this, &UMainMenuWidget::UnhoverButton);
  LoadMenuButton->OnClicked.AddDynamic(this, &UMainMenuWidget::LoadMenu);
  LoadMenuButton->OnHovered.AddDynamic(this, &UMainMenuWidget::UnhoverButton);
  SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::SettingsMenu);
  SettingsButton->OnHovered.AddDynamic(this, &UMainMenuWidget::UnhoverButton);
  ExitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::Exit);
  ExitButton->OnHovered.AddDynamic(this, &UMainMenuWidget::UnhoverButton);

  SetupUIActionable();
}

void UMainMenuWidget::SelectHoveredButton() {
  if (!HoveredButton) return;

  if (HoveredButton == ContinueButton) Continue();
  else if (HoveredButton == NewGameButton) NewGame();
  else if (HoveredButton == LoadMenuButton) LoadMenu();
  else if (HoveredButton == SettingsButton) SettingsMenu();
  else if (HoveredButton == ExitButton) Exit();
}
void UMainMenuWidget::HoverButton(UButton* Button) {
  check(Button);
  if (HoveredButton == Button) return;

  if (HoveredButton) {
    FButtonStyle ButtonStyle = Button->GetStyle();
    ButtonStyle.SetNormal(ButtonStyle.Normal);
    HoveredButton->SetStyle(ButtonStyle);
  }
  HoveredButton = Button;
  FButtonStyle ButtonStyle = HoveredButton->GetStyle();
  ButtonStyle.SetNormal(ButtonStyle.Hovered);
  HoveredButton->SetStyle(ButtonStyle);
  // HoveredButton->SetFocus();

  UGameplayStatics::PlaySound2D(this, HoverSound, 1.0f);
}
void UMainMenuWidget::HoverNextButton(FVector2D Direction) {
  if (Direction.X == 0) return;

  if (!HoveredButton) {
    HoverButton(NewGameButton);
    return;
  }

  TArray<UButton*> Buttons = {ContinueButton, NewGameButton, LoadMenuButton, SettingsButton, ExitButton};
  int32 CurrentIndex = Buttons.IndexOfByKey(HoveredButton);
  check(CurrentIndex != INDEX_NONE);
  int32 NextIndex = CurrentIndex + (Direction.X > 0 ? 1 : -1);
  if (NextIndex < 0 || NextIndex >= Buttons.Num()) return;
  if (!Buttons[NextIndex]->GetIsEnabled()) return;

  HoverButton(Buttons[NextIndex]);
}
void UMainMenuWidget::UnhoverButton() {
  if (!HoveredButton) return;

  FButtonStyle ButtonStyle = HoveredButton == NewGameButton ? LoadMenuButton->GetStyle() : NewGameButton->GetStyle();
  ButtonStyle.SetNormal(ButtonStyle.Normal);
  HoveredButton->SetStyle(ButtonStyle);
  HoveredButton = nullptr;
}

void UMainMenuWidget::Continue() {
  if (SaveManager->SaveSlotListSaveGame->MostRecentSaveSlotIndex == -1 ||
      SaveManager->SaveSlotListSaveGame->SaveSlotList.Num() <= 0)
    return;

  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);

  AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  check(MainMenuGameMode);
  MainMenuGameMode->Continue();
}
void UMainMenuWidget::NewGame() {
  NewGameSetupWidget->InitUI(InUIInputActions, SettingsManager, [this]() {
    MainOverlay->SetVisibility(ESlateVisibility::Visible);
    NewGameSetupWidget->SetVisibility(ESlateVisibility::Collapsed);
    HoverButton(NewGameButton);
  });
  NewGameSetupWidget->RefreshUI();

  MainOverlay->SetVisibility(ESlateVisibility::Hidden);
  NewGameSetupWidget->SetVisibility(ESlateVisibility::Visible);
  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);

  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);
}
void UMainMenuWidget::LoadMenu() {
  auto BackFunc = [this]() {
    MainOverlay->SetVisibility(ESlateVisibility::Visible);
    SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
    HoverButton(LoadMenuButton);
  };
  SaveLoadSlotsWidget->InitUI(InUIInputActions, false, SaveManager, BackFunc);
  SaveLoadSlotsWidget->RefreshUI();

  MainOverlay->SetVisibility(ESlateVisibility::Hidden);
  NewGameSetupWidget->SetVisibility(ESlateVisibility::Collapsed);
  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Visible);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);

  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);
}
void UMainMenuWidget::SettingsMenu() {
  auto BackFunc = [this]() {
    MainOverlay->SetVisibility(ESlateVisibility::Visible);
    SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
    HoverButton(SettingsButton);
  };
  SettingsWidget->InitUI(InUIInputActions, SettingsManager, BackFunc);

  MainOverlay->SetVisibility(ESlateVisibility::Hidden);
  NewGameSetupWidget->SetVisibility(ESlateVisibility::Collapsed);
  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Visible);

  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);
}
void UMainMenuWidget::Exit() {
  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);

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

  NewGameSetupWidget->SetVisibility(ESlateVisibility::Collapsed);
  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);

  HoverButton(NewGameButton);
}

void UMainMenuWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() {
    if (NewGameSetupWidget->IsVisible()) NewGameSetupWidget->UIActionable.AdvanceUI();
    else if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.AdvanceUI();
    else if (SettingsWidget->IsVisible()) SettingsWidget->UIActionable.AdvanceUI();
    else SelectHoveredButton();
  };
  UIActionable.DirectionalInput = [this](FVector2D Direction) {
    if (NewGameSetupWidget->IsVisible()) NewGameSetupWidget->UIActionable.DirectionalInput(Direction);
    else if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.DirectionalInput(Direction);
    else if (SettingsWidget->IsVisible()) SettingsWidget->UIActionable.DirectionalInput(Direction);
    else HoverNextButton(Direction);
  };
  UIActionable.SideButton4 = [this]() {
    if (NewGameSetupWidget->IsVisible()) NewGameSetupWidget->UIActionable.SideButton4();
    else if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.SideButton4();
    else if (SettingsWidget->IsVisible()) SettingsWidget->UIActionable.SideButton4();
  };
  UIActionable.RetractUI = [this]() {
    if (NewGameSetupWidget->IsVisible()) NewGameSetupWidget->UIActionable.RetractUI();
    else if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.RetractUI();
    else if (SettingsWidget->IsVisible()) SettingsWidget->UIActionable.RetractUI();
  };
}