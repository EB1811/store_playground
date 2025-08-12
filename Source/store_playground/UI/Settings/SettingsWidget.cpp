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
  GameSettingsButton->OnHovered.AddDynamic(this, &USettingsWidget::UnhoverButton);
  DisplaySettingsButton->OnClicked.AddDynamic(this, &USettingsWidget::DisplaySettings);
  DisplaySettingsButton->OnHovered.AddDynamic(this, &USettingsWidget::UnhoverButton);
  GraphicsSettingsButton->OnClicked.AddDynamic(this, &USettingsWidget::GraphicsSettings);
  GraphicsSettingsButton->OnHovered.AddDynamic(this, &USettingsWidget::UnhoverButton);
  SoundSettingsButton->OnClicked.AddDynamic(this, &USettingsWidget::SoundSettings);
  SoundSettingsButton->OnHovered.AddDynamic(this, &USettingsWidget::UnhoverButton);
  ControlsSettingsButton->OnClicked.AddDynamic(this, &USettingsWidget::ControlsSettings);
  ControlsSettingsButton->OnHovered.AddDynamic(this, &USettingsWidget::UnhoverButton);
  BackButton->OnClicked.AddDynamic(this, &USettingsWidget::Back);
  BackButton->OnHovered.AddDynamic(this, &USettingsWidget::UnhoverButton);

  SetupUIActionable();
}

void USettingsWidget::SelectHoveredButton() {
  if (!HoveredButton) return;

  if (HoveredButton == GameSettingsButton) GameSettings();
  else if (HoveredButton == DisplaySettingsButton) DisplaySettings();
  else if (HoveredButton == GraphicsSettingsButton) GraphicsSettings();
  else if (HoveredButton == SoundSettingsButton) SoundSettings();
  else if (HoveredButton == ControlsSettingsButton) ControlsSettings();
  else if (HoveredButton == BackButton) Back();
}
void USettingsWidget::HoverButton(UButton* Button) {
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
void USettingsWidget::HoverNextButton(FVector2D Direction) {
  if (Direction.X == 0) return;

  if (!HoveredButton) {
    HoverButton(GameSettingsButton);
    return;
  }

  TArray<UButton*> Buttons = {GameSettingsButton,  DisplaySettingsButton,  GraphicsSettingsButton,
                              SoundSettingsButton, ControlsSettingsButton, BackButton};
  int32 CurrentIndex = Buttons.IndexOfByKey(HoveredButton);
  check(CurrentIndex != INDEX_NONE);
  int32 NextIndex = CurrentIndex + (Direction.X > 0 ? 1 : -1);
  if (NextIndex < 0 || NextIndex >= Buttons.Num()) return;

  HoverButton(Buttons[NextIndex]);
}
void USettingsWidget::UnhoverButton() {
  if (!HoveredButton) return;

  FButtonStyle ButtonStyle =
      HoveredButton == GameSettingsButton ? DisplaySettingsButton->GetStyle() : GameSettingsButton->GetStyle();
  ButtonStyle.SetNormal(ButtonStyle.Normal);
  HoveredButton->SetStyle(ButtonStyle);
  HoveredButton = nullptr;
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
      // Restore button hovering when returning to main settings
      if (!HoveredButton) HoverButton(GameSettingsButton);
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

  HoverButton(GameSettingsButton);
}

void USettingsWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() {
    if (CurrentCategory == ESettingsCategory::None) SelectHoveredButton();
    else if (CurrentCategory == ESettingsCategory::Display) DisplaySettingsWidget->UIActionable.AdvanceUI();
    else if (CurrentCategory == ESettingsCategory::Graphics) GraphicsSettingsWidget->UIActionable.AdvanceUI();
    else if (CurrentCategory == ESettingsCategory::Sound) SoundSettingsWidget->UIActionable.AdvanceUI();
  };
  UIActionable.DirectionalInput = [this](FVector2D Direction) {
    if (CurrentCategory == ESettingsCategory::None) HoverNextButton(Direction);
  };
  UIActionable.SideButton4 = [this]() {};
  UIActionable.RetractUI = [this]() {
    if (CurrentCategory == ESettingsCategory::None) Back();
    else if (CurrentCategory == ESettingsCategory::Display) DisplaySettingsWidget->UIActionable.RetractUI();
    else if (CurrentCategory == ESettingsCategory::Graphics) GraphicsSettingsWidget->UIActionable.RetractUI();
    else if (CurrentCategory == ESettingsCategory::Sound) SoundSettingsWidget->UIActionable.RetractUI();
  };
}