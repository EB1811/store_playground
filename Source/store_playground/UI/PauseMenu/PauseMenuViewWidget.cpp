#include "PauseMenuViewWidget.h"
#include "store_playground/UI/SaveSlots/SaveLoadSlotsWidget.h"
#include "store_playground/UI/Settings/SettingsWidget.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UPauseMenuViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuViewWidget::Resume);
  SaveButton->OnClicked.AddDynamic(this, &UPauseMenuViewWidget::SaveMenu);
  LoadButton->OnClicked.AddDynamic(this, &UPauseMenuViewWidget::LoadMenu);
  SettingsButton->OnClicked.AddDynamic(this, &UPauseMenuViewWidget::SettingsMenu);
  QuitButton->OnClicked.AddDynamic(this, &UPauseMenuViewWidget::Quit);

  SetupUIActionable();
}

void UPauseMenuViewWidget::Resume() { CloseWidgetFunc(); }

void UPauseMenuViewWidget::SaveMenu() {
  if (!SaveManager->CanSave()) return;

  auto BackFunc = [this]() { SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed); };
  SaveLoadSlotsWidget->InitUI(InUIInputActions, true, SaveManager, BackFunc);
  SaveLoadSlotsWidget->RefreshUI();

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Visible);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}
void UPauseMenuViewWidget::LoadMenu() {
  auto BackFunc = [this]() { SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed); };
  SaveLoadSlotsWidget->InitUI(InUIInputActions, false, SaveManager, BackFunc);
  SaveLoadSlotsWidget->RefreshUI();

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Visible);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}
void UPauseMenuViewWidget::SettingsMenu() {
  auto BackFunc = [this]() { SettingsWidget->SetVisibility(ESlateVisibility::Collapsed); };
  // SettingsWidget->InitUI(false, SaveManager, BackFunc);

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Visible);
}
void UPauseMenuViewWidget::Quit() {}

void UPauseMenuViewWidget::RefreshUI() {
  if (SaveManager->CanSave()) SaveButton->SetIsEnabled(true);
  else SaveButton->SetIsEnabled(false);
}

void UPauseMenuViewWidget::InitUI(FInUIInputActions _InUIInputActions,
                                  class ASaveManager* _SaveManager,
                                  std::function<void()> _CloseWidgetFunc) {
  check(_SaveManager && _CloseWidgetFunc);

  InUIInputActions = _InUIInputActions;
  SaveManager = _SaveManager;
  CloseWidgetFunc = _CloseWidgetFunc;

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UPauseMenuViewWidget::SetupUIActionable() {
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
    else CloseWidgetFunc();
  };
  UIActionable.QuitUI = [this]() { CloseWidgetFunc(); };
}