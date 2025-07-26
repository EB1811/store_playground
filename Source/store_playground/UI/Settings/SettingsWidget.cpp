#include "SettingsWidget.h"
#include "CoreFwd.h"
#include "Logging/LogVerbosity.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/UI/Settings/SoundSettingsWidget.h"
#include "store_playground/UI/Settings/DisplaySettingsWidget.h"
#include "store_playground/UI/Settings/GraphicsSettingsWidget.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/UI/Inventory/ItemsWidget.h"
#include "store_playground/UI/Components/RightSlideWidget.h"
#include "store_playground/UI/Components/LeftSlideWidget.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Components/ControlTextWidget.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Kismet/GameplayStatics.h"

void USettingsWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  GameSettingsButton->OnClicked.AddDynamic(this, &USettingsWidget::GameSettings);
  DisplaySettingsButton->OnClicked.AddDynamic(this, &USettingsWidget::DisplaySettings);
  GraphicsSettingsButton->OnClicked.AddDynamic(this, &USettingsWidget::GraphicsSettings);
  SoundSettingsButton->OnClicked.AddDynamic(this, &USettingsWidget::SoundSettings);
  ControlsSettingsButton->OnClicked.AddDynamic(this, &USettingsWidget::ControlsSettings);
  BackButton->OnClicked.AddDynamic(this, &USettingsWidget::Back);
}

void USettingsWidget::GameSettings() {
  CurrentCategory = ESettingsCategory::Game;

  RefreshUI();
}
void USettingsWidget::DisplaySettings() {
  CurrentCategory = ESettingsCategory::Display;

  RefreshUI();
}
void USettingsWidget::GraphicsSettings() {
  CurrentCategory = ESettingsCategory::Graphics;

  RefreshUI();
}
void USettingsWidget::SoundSettings() {
  CurrentCategory = ESettingsCategory::Sound;

  RefreshUI();
}
void USettingsWidget::ControlsSettings() {
  CurrentCategory = ESettingsCategory::Controls;

  RefreshUI();
}

void USettingsWidget::Back() {
  CurrentCategory = ESettingsCategory::None;

  CloseWidgetFunc();
}

void USettingsWidget::RefreshUI() {
  switch (CurrentCategory) {
    case ESettingsCategory::None:
      SettingsOverlay->SetVisibility(ESlateVisibility::Visible);
      if (OpenedWidget) OpenedWidget->SetVisibility(ESlateVisibility::Collapsed);
      break;
    case ESettingsCategory::Game:
      SettingsOverlay->SetVisibility(ESlateVisibility::Hidden);
      if (OpenedWidget) OpenedWidget->SetVisibility(ESlateVisibility::Collapsed);
      break;
    case ESettingsCategory::Display:
      SettingsOverlay->SetVisibility(ESlateVisibility::Hidden);
      if (OpenedWidget) OpenedWidget->SetVisibility(ESlateVisibility::Collapsed);

      OpenedWidget = DisplaySettingsWidget;
      DisplaySettingsWidget->RefreshUI();
      DisplaySettingsWidget->SetVisibility(ESlateVisibility::Visible);
      break;
    case ESettingsCategory::Graphics:
      SettingsOverlay->SetVisibility(ESlateVisibility::Hidden);
      if (OpenedWidget) OpenedWidget->SetVisibility(ESlateVisibility::Collapsed);

      OpenedWidget = GraphicsSettingsWidget;
      GraphicsSettingsWidget->RefreshUI();
      GraphicsSettingsWidget->SetVisibility(ESlateVisibility::Visible);
      break;
    case ESettingsCategory::Sound:
      SettingsOverlay->SetVisibility(ESlateVisibility::Hidden);
      if (OpenedWidget) OpenedWidget->SetVisibility(ESlateVisibility::Collapsed);

      OpenedWidget = SoundSettingsWidget;
      SoundSettingsWidget->RefreshUI();
      SoundSettingsWidget->SetVisibility(ESlateVisibility::Visible);
      break;
    case ESettingsCategory::Controls:
      SettingsOverlay->SetVisibility(ESlateVisibility::Hidden);
      if (OpenedWidget) OpenedWidget->SetVisibility(ESlateVisibility::Collapsed);
      break;
    default: checkNoEntry(); break;
  }
}

void USettingsWidget::InitUI(FInUIInputActions _InUIInputActions,
                             ASettingsManager* _SettingsManager,
                             std::function<void()> _CloseWidgetFunc) {
  check(_SettingsManager && _CloseWidgetFunc);

  SettingsManager = _SettingsManager;
  CloseWidgetFunc = _CloseWidgetFunc;

  CurrentCategory = ESettingsCategory::None;
  OpenedWidget = nullptr;

  SoundSettingsWidget->InitUI(_InUIInputActions, SettingsManager, [this]() {
    CurrentCategory = ESettingsCategory::None;
    RefreshUI();
  });
  SoundSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);

  DisplaySettingsWidget->InitUI(_InUIInputActions, SettingsManager, [this]() {
    CurrentCategory = ESettingsCategory::None;
    RefreshUI();
  });
  DisplaySettingsWidget->SetVisibility(ESlateVisibility::Collapsed);

  GraphicsSettingsWidget->InitUI(_InUIInputActions, SettingsManager, [this]() {
    CurrentCategory = ESettingsCategory::None;
    RefreshUI();
  });
  GraphicsSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}