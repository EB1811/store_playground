#include "UpgradeWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UUpgradeWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  if (UpgradeButton) UpgradeButton->OnClicked.AddDynamic(this, &UUpgradeWidget::OnUpgradeButtonClicked);
}

void UUpgradeWidget::SetUpgradeData(const FUpgrade& Upgrade) {
  UpgradeID = Upgrade.ID;

  UpgradeNameText->SetText(Upgrade.TextData.Name);
  UpgradeDescText->SetText(Upgrade.TextData.Description);

  // Set image if we have one
  // if (ArticleImage && Upgrade.AssetData.Icon)
  // {
  //   ArticleImage->SetBrushFromTexture(Upgrade.AssetData.Icon);
  // }
}

void UUpgradeWidget::OnUpgradeButtonClicked() {
  check(SelectUpgradeFunc);

  SelectUpgradeFunc(UpgradeID);
}
