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
}

void UPauseMenuViewWidget::Resume() { CloseWidgetFunc(); }

void UPauseMenuViewWidget::SaveMenu() {
  auto BackFunc = [this]() { SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed); };
  SaveLoadSlotsWidget->InitUI(true, SaveManager, BackFunc);
  SaveLoadSlotsWidget->RefreshUI();

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Visible);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}
void UPauseMenuViewWidget::LoadMenu() {
  auto BackFunc = [this]() { SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed); };
  SaveLoadSlotsWidget->InitUI(false, SaveManager, BackFunc);
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

void UPauseMenuViewWidget::RefreshUI() {}

void UPauseMenuViewWidget::InitUI(FInUIInputActions InUIInputActions,
                                  class ASaveManager* _SaveManager,
                                  std::function<void()> _CloseWidgetFunc) {
  check(_SaveManager && _CloseWidgetFunc);

  SaveManager = _SaveManager;
  CloseWidgetFunc = _CloseWidgetFunc;

  SaveLoadSlotsWidget->SetVisibility(ESlateVisibility::Collapsed);
  SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}