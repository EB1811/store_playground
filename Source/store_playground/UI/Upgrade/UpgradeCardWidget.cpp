#include "UpgradeCardWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/Border.h"

void UUpgradeCardWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SelectButton->OnClicked.AddDynamic(this, &UUpgradeCardWidget::OnSelectButtonClicked);
}

void UUpgradeCardWidget::RefreshUI() {
  if (bIsSelected) {
    UpgradeDescText->SetVisibility(ESlateVisibility::Visible);
    CardBorder->SetVisibility(ESlateVisibility::Visible);
    // SelectButton->SetIsEnabled(false);
  } else {
    UpgradeDescText->SetVisibility(ESlateVisibility::Collapsed);
    CardBorder->SetVisibility(ESlateVisibility::Collapsed);
    // SelectButton->SetIsEnabled(true);
  }
}

void UUpgradeCardWidget::InitUI(const FUpgrade& Upgrade,
                                TArray<FUpgradeEffect> UpgradeEffects,
                                std::function<void(FName)> _SelectFunc) {
  check(_SelectFunc);

  UpgradeID = Upgrade.ID;
  SelectFunc = _SelectFunc;

  UpgradeNameText->SetText(Upgrade.TextData.Name);
  UpgradeDescText->SetText(Upgrade.TextData.Description);
  UpgradeCostText->SetText(FText::FromString("Cost: " + FString::FromInt(Upgrade.Cost)));

  TArray<FText> UpgradeEffectsTexts;
  for (const FUpgradeEffect& Effect : UpgradeEffects)
    if (!Effect.TextData.Description.IsEmpty()) UpgradeEffectsTexts.Add(Effect.TextData.Description);
  UpgradeEffectsRichText->SetText(FText::Join(FText::FromString("\n"), UpgradeEffectsTexts));
}

void UUpgradeCardWidget::OnSelectButtonClicked() {
  check(SelectFunc);

  SelectFunc(UpgradeID);
}
