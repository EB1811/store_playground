#include "UpgradeListWidget.h"
#include "UpgradeWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
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

  AvailableUpgradePointsText->SetText(FText::AsNumber(UpgradeManagerRef->AvailableUpgradePoints));

  UpgradeListPanelWrapBox->ClearChildren();
  ReqsNotMetPanelWrapBox->ClearChildren();

  TArray<FUpgrade> AvailableUpgrades = UpgradeManagerRef->GetAvailableUpgrades(UpgradeClass);
  for (const FUpgrade& Upgrade : AvailableUpgrades) {
    if (UUpgradeWidget* UpgradeWidget = CreateWidget<UUpgradeWidget>(GetWorld(), UpgradeWidgetClass)) {
      if (UpgradeManagerRef->AvailableUpgradePoints <= 0) UpgradeWidget->UpgradeButton->SetIsEnabled(false);
      else UpgradeWidget->UpgradeButton->SetIsEnabled(true);

      UpgradeWidget->SelectUpgradeFunc = [this](FName UpgradeID) {
        SelectUpgradeFunc(UpgradeID);
        RefreshUpgradeListUI();
      };
      UpgradeWidget->SetUpgradeData(Upgrade);
      UpgradeListPanelWrapBox->AddChildToWrapBox(UpgradeWidget);
    }
  }
  TArray<FUpgrade> ReqsNotMetUpgrades = UpgradeManagerRef->GetUpgradesReqsNotMet(UpgradeClass);
  for (const FUpgrade& Upgrade : ReqsNotMetUpgrades) {
    if (UUpgradeWidget* UpgradeWidget = CreateWidget<UUpgradeWidget>(GetWorld(), UpgradeWidgetClass)) {
      UpgradeWidget->UpgradeButton->SetIsEnabled(false);

      UpgradeWidget->SetUpgradeData(Upgrade);
      ReqsNotMetPanelWrapBox->AddChildToWrapBox(UpgradeWidget);
    }
  }
}
