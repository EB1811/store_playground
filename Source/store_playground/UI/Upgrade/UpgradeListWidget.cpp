#include "UpgradeListWidget.h"
#include "UpgradeWidget.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "store_playground/Upgrade/UpgradeManager.h"

void UUpgradeListWidget::SetUpgradeClass(EUpgradeClass NewUpgradeClass, const FText& Title, const FText& Description) {
  UpgradeClass = NewUpgradeClass;

  UpgradeListNameText->SetText(Title);
  UpgradeListDescText->SetText(Description);

  RefreshUpgradeListUI();
}

void UUpgradeListWidget::RefreshUpgradeListUI() {
  check(UpgradeManagerRef && SelectUpgradeFunc);

  UpgradeListPanelWrapBox->ClearChildren();

  TArray<FUpgrade> AvailableUpgrades = UpgradeManagerRef->GetAvailableUpgrades(UpgradeClass);
  for (const FUpgrade& Upgrade : AvailableUpgrades) {
    if (UUpgradeWidget* UpgradeWidget = CreateWidget<UUpgradeWidget>(GetWorld(), UpgradeWidgetClass)) {
      UpgradeWidget->SelectUpgradeFunc = [this](FName UpgradeID) {
        SelectUpgradeFunc(UpgradeID);
        RefreshUpgradeListUI();
      };
      UpgradeWidget->SetUpgradeData(Upgrade);
      UpgradeListPanelWrapBox->AddChildToWrapBox(UpgradeWidget);
    }
  }
}
