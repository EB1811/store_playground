#include "UpgradeViewWidget.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Components/ControlTextWidget.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "store_playground/UI/Upgrade/PowerUpgradesWidget.h"
#include "store_playground/Upgrade/UpgradeSelectComponent.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UUpgradeViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  UnlockButton->ControlButton->OnClicked.AddDynamic(this, &UUpgradeViewWidget::Unlock);
  BackButton->ControlButton->OnClicked.AddDynamic(this, &UUpgradeViewWidget::Back);
}

void UUpgradeViewWidget::Unlock() {
  if (UpgradeManager->AvailableUpgradePoints <= 0) return;
  if (RelevantPowerUpgradesWidget->SelectedUpgradeID.IsNone()) return;

  RelevantPowerUpgradesWidget->UnlockUpgrade();
}

void UUpgradeViewWidget::Back() { CloseWidgetFunc(); }

void UUpgradeViewWidget::RefreshUI() {
  check(RelevantPowerUpgradesWidget);
  RelevantPowerUpgradesWidget->RefreshUI();
}

void UUpgradeViewWidget::InitUI(FInputActions InputActions,
                                class AUpgradeManager* _UpgradeManager,
                                class UUpgradeSelectComponent* UpgradeSelectC,
                                std::function<void()> _CloseWidgetFunc) {
  check(_UpgradeManager && UpgradeSelectC && _CloseWidgetFunc);

  UpgradeManager = _UpgradeManager;
  UpgradeClass = UpgradeSelectC->UpgradeClass;

  Power1UpgradeWidget->SetVisibility(ESlateVisibility::Collapsed);
  Power2UpgradeWidget->SetVisibility(ESlateVisibility::Collapsed);
  Power3UpgradeWidget->SetVisibility(ESlateVisibility::Collapsed);
  Power4UpgradeWidget->SetVisibility(ESlateVisibility::Collapsed);
  switch (UpgradeClass) {
    case EUpgradeClass::Holy: RelevantPowerUpgradesWidget = Power1UpgradeWidget; break;
    case EUpgradeClass::Demonic: RelevantPowerUpgradesWidget = Power2UpgradeWidget; break;
    case EUpgradeClass::Artisanal: RelevantPowerUpgradesWidget = Power3UpgradeWidget; break;
    case EUpgradeClass::Eldritch: RelevantPowerUpgradesWidget = Power4UpgradeWidget; break;
    default: checkNoEntry();
  }
  RelevantPowerUpgradesWidget->InitUI(UpgradeClass, UpgradeManager);
  RelevantPowerUpgradesWidget->SetVisibility(ESlateVisibility::Visible);

  UnlockButton->ActionText->SetText(FText::FromString("Unlock"));
  BackButton->ActionText->SetText(FText::FromString("Back"));

  CloseWidgetFunc = _CloseWidgetFunc;
}