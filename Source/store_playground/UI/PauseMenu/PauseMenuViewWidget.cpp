#include "PauseMenuViewWidget.h"
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

void UPauseMenuViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuViewWidget::Resume);
  ResumeButton->OnHovered.AddDynamic(this, &UPauseMenuViewWidget::UnhoverButton);
  SaveButton->OnClicked.AddDynamic(this, &UPauseMenuViewWidget::SaveMenu);
  SaveButton->OnHovered.AddDynamic(this, &UPauseMenuViewWidget::UnhoverButton);
  LoadButton->OnClicked.AddDynamic(this, &UPauseMenuViewWidget::LoadMenu);
  LoadButton->OnHovered.AddDynamic(this, &UPauseMenuViewWidget::UnhoverButton);
  SettingsButton->OnClicked.AddDynamic(this, &UPauseMenuViewWidget::SettingsMenu);
  SettingsButton->OnHovered.AddDynamic(this, &UPauseMenuViewWidget::UnhoverButton);
  QuitButton->OnClicked.AddDynamic(this, &UPauseMenuViewWidget::Quit);
  QuitButton->OnHovered.AddDynamic(this, &UPauseMenuViewWidget::UnhoverButton);

  SetupUIActionable();
}

void UPauseMenuViewWidget::SelectHoveredButton() {
  if (!HoveredButton) return;

  if (HoveredButton == ResumeButton) Resume();
  else if (HoveredButton == SaveButton) SaveMenu();
  else if (HoveredButton == LoadButton) LoadMenu();
  else if (HoveredButton == SettingsButton) SettingsMenu();
  else if (HoveredButton == QuitButton) Quit();
}
void UPauseMenuViewWidget::HoverButton(UButton* Button) {
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
  HoveredButton->SetFocus();
}
void UPauseMenuViewWidget::HoverNextButton(FVector2D Direction) {
  if (Direction.X == 0) return;

  if (!HoveredButton) {
    HoverButton(ResumeButton);
    return;
  }

  TArray<UButton*> Buttons = {ResumeButton, SaveButton, LoadButton, SettingsButton, QuitButton};
  int32 CurrentIndex = Buttons.IndexOfByKey(HoveredButton);
  check(CurrentIndex != INDEX_NONE);
  int32 NextIndex = CurrentIndex + (Direction.X > 0 ? 1 : -1);
  if (NextIndex < 0 || NextIndex >= Buttons.Num()) return;

  HoverButton(Buttons[NextIndex]);
}
void UPauseMenuViewWidget::UnhoverButton() {
  if (!HoveredButton) return;

  FButtonStyle ButtonStyle = HoveredButton == ResumeButton ? SaveButton->GetStyle() : ResumeButton->GetStyle();
  ButtonStyle.SetNormal(ButtonStyle.Normal);
  HoveredButton->SetStyle(ButtonStyle);
  HoveredButton = nullptr;
}

void UPauseMenuViewWidget::Resume() { CloseWidgetFunc(); }

void UPauseMenuViewWidget::SaveMenu() {
  if (!SaveManager->CanSave()) return;

  auto BackFunc = [this]() {
    SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
    HoverButton(SaveButton);
  };
  SaveLoadSlotsWidget->InitUI(InUIInputActions, true, SaveManager, BackFunc);
  SaveLoadSlotsWidget->RefreshUI();

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Visible);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}
void UPauseMenuViewWidget::LoadMenu() {
  auto BackFunc = [this]() {
    SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
    HoverButton(LoadButton);
  };
  SaveLoadSlotsWidget->InitUI(InUIInputActions, false, SaveManager, BackFunc);
  SaveLoadSlotsWidget->RefreshUI();

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Visible);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}
void UPauseMenuViewWidget::SettingsMenu() {
  auto BackFunc = [this]() {
    SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
    HoverButton(SettingsButton);
  };
  SettingsWidget->InitUI(InUIInputActions, SettingsManager, BackFunc);
  SettingsWidget->RefreshUI();

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Visible);
}
void UPauseMenuViewWidget::Quit() {
  AStorePGGameMode* GameMode = Cast<AStorePGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  check(GameMode);
  GameMode->ExitToMainMenu();
}

void UPauseMenuViewWidget::RefreshUI() {
  if (SaveManager->CanSave()) SaveButton->SetIsEnabled(true);
  else SaveButton->SetIsEnabled(false);
}

void UPauseMenuViewWidget::InitUI(FInUIInputActions _InUIInputActions,
                                  class ASettingsManager* _SettingsManager,
                                  class ASaveManager* _SaveManager,
                                  std::function<void()> _CloseWidgetFunc) {
  check(_SettingsManager && _SaveManager && _CloseWidgetFunc);

  InUIInputActions = _InUIInputActions;
  SettingsManager = _SettingsManager;
  SaveManager = _SaveManager;
  CloseWidgetFunc = _CloseWidgetFunc;

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);

  HoverButton(ResumeButton);
}

void UPauseMenuViewWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() {
    if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.AdvanceUI();
    else if (SettingsWidget->IsVisible()) SettingsWidget->UIActionable.AdvanceUI();
    else SelectHoveredButton();
  };
  UIActionable.DirectionalInput = [this](FVector2D Direction) {
    if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.DirectionalInput(Direction);
    else if (SettingsWidget->IsVisible()) SettingsWidget->UIActionable.DirectionalInput(Direction);
    else HoverNextButton(Direction);
  };
  UIActionable.SideButton4 = [this]() {
    if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.SideButton4();
    else if (SettingsWidget->IsVisible()) SettingsWidget->UIActionable.SideButton4();
  };
  UIActionable.RetractUI = [this]() {
    if (SaveLoadSlotsWidget->IsVisible()) SaveLoadSlotsWidget->UIActionable.RetractUI();
    else if (SettingsWidget->IsVisible()) SettingsWidget->UIActionable.RetractUI();
    else CloseWidgetFunc();
  };
  UIActionable.QuitUI = [this]() { CloseWidgetFunc(); };
}