#include "PowerUpgradesWidget.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/UI/Upgrade/UpgradeCardWidget.h"
#include "store_playground/UI/Upgrade/DisabledUpgradeCardWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/VerticalBox.h"

void UPowerUpgradesWidget::SelectUpgrade(FName UpgradeID, UUpgradeCardWidget* UpgradeCardWidget) {
  check(UpgradeCardWidget);

  if (SelectedUpgradeID == UpgradeID) return;

  SelectedUpgradeID = UpgradeID;

  if (SelectedUpgradeCardWidget) {
    SelectedUpgradeCardWidget->bIsSelected = false;
    SelectedUpgradeCardWidget->RefreshUI();
  }
  SelectedUpgradeCardWidget = UpgradeCardWidget;
  SelectedUpgradeCardWidget->bIsSelected = true;
  SelectedUpgradeCardWidget->RefreshUI();
}

void UPowerUpgradesWidget::UnlockUpgrade() {
  if (SelectedUpgradeID.IsNone()) return;

  TArray<FUpgrade> UnlockableUpgrades = UpgradeManager->GetAvailableUpgrades(UpgradeClass);
  FUpgrade* Upgrade =
      UnlockableUpgrades.FindByPredicate([&](const FUpgrade& Upgrade) { return Upgrade.ID == SelectedUpgradeID; });
  checkf(Upgrade && Upgrade->Cost <= UpgradeManager->AvailableUpgradePoints,
         TEXT("Selected upgrade is not available or cost exceeds available points, this means "
              "GetAvailableUpgrades returned wrong data."));

  UpgradeManager->SelectUpgrade(SelectedUpgradeID);
  SelectedUpgradeID = FName();
  SelectedUpgradeCardWidget = nullptr;
  RefreshUI();
}

void UPowerUpgradesWidget::RefreshUI() {
  AvailableUpgradePointsText->SetText(
      FText::FromString(FString::Printf(TEXT("Points Available: %d"), UpgradeManager->AvailableUpgradePoints)));

  UpgradableUpgradesBox->ClearChildren();
  LockedUpgradesBox->ClearChildren();
  UnlockedUpgradesBox->ClearChildren();

  TArray<FUpgrade> UnlockableUpgrades = UpgradeManager->GetAvailableUpgrades(UpgradeClass);
  UnlockableUpgrades = UnlockableUpgrades.FilterByPredicate(
      [&](const FUpgrade& Upgrade) { return Upgrade.Cost <= UpgradeManager->AvailableUpgradePoints; });
  UnlockableUpgrades.Sort([](const FUpgrade& A, const FUpgrade& B) { return A.Cost < B.Cost; });

  if (UnlockableUpgrades.Num() > 0 && SelectedUpgradeID.IsNone()) SelectedUpgradeID = UnlockableUpgrades[0].ID;

  for (const FUpgrade& Upgrade : UnlockableUpgrades) {
    UUpgradeCardWidget* UpgradeCardWidget = CreateWidget<UUpgradeCardWidget>(this, UpgradeCardWidgetClass);
    check(UpgradeCardWidget);

    if (Upgrade.ID == SelectedUpgradeID) {
      UpgradeCardWidget->bIsSelected = true;
      SelectedUpgradeCardWidget = UpgradeCardWidget;
    }

    UpgradeCardWidget->InitUI(
        Upgrade, UpgradeManager->GetUpgradeEffectsByIds(Upgrade.UpgradeEffectIDs),
        [this, UpgradeCardWidget](FName UpgradeId) { SelectUpgrade(UpgradeId, UpgradeCardWidget); });
    UpgradeCardWidget->RefreshUI();
    UpgradableUpgradesBox->AddChildToVerticalBox(UpgradeCardWidget);
  }

  TArray<FUpgrade> LockedUpgrades = UpgradeManager->GetAvailableUpgrades(UpgradeClass);
  LockedUpgrades = LockedUpgrades.FilterByPredicate(
      [&](const FUpgrade& Upgrade) { return Upgrade.Cost > UpgradeManager->AvailableUpgradePoints; });
  LockedUpgrades.Sort([](const FUpgrade& A, const FUpgrade& B) { return A.Cost < B.Cost; });
  TArray<FUpgrade> ReqsNotMetUpgrades = UpgradeManager->GetUpgradesReqsNotMet(UpgradeClass);
  ReqsNotMetUpgrades.Sort([](const FUpgrade& A, const FUpgrade& B) { return A.Cost < B.Cost; });
  LockedUpgrades.Append(ReqsNotMetUpgrades);
  for (const FUpgrade& Upgrade : LockedUpgrades) {
    UDisabledUpgradeCardWidget* DisabledUpgradeCardWidget =
        CreateWidget<UDisabledUpgradeCardWidget>(this, DisabledUpgradeCardWidgetClass);
    check(DisabledUpgradeCardWidget);

    DisabledUpgradeCardWidget->UpgradeNameText->SetText(Upgrade.TextData.Name);
    DisabledUpgradeCardWidget->UpgradeCostText->SetText(FText::FromString("Cost: " + FString::FromInt(Upgrade.Cost)));
    DisabledUpgradeCardWidget->UpgradeCostText->SetVisibility(ESlateVisibility::Visible);
    if (Upgrade.Requirements.IsNone()) {
      DisabledUpgradeCardWidget->ReqsNotMetText->SetText(FText::FromString(""));
      DisabledUpgradeCardWidget->ReqsNotMetText->SetVisibility(ESlateVisibility::Collapsed);
    } else {
      DisabledUpgradeCardWidget->ReqsNotMetText->SetText(Upgrade.RequirementsFilterDescription);
      DisabledUpgradeCardWidget->ReqsNotMetText->SetVisibility(ESlateVisibility::Visible);
    }

    LockedUpgradesBox->AddChildToVerticalBox(DisabledUpgradeCardWidget);
  }

  TArray<FUpgrade> UnlockedUpgrades = UpgradeManager->GetSelectedUpgrades(UpgradeClass);
  UnlockedUpgrades.Sort([](const FUpgrade& A, const FUpgrade& B) { return A.Cost < B.Cost; });
  for (const FUpgrade& Upgrade : UnlockedUpgrades) {
    UDisabledUpgradeCardWidget* UnlockedUpgradeCardWidget =
        CreateWidget<UDisabledUpgradeCardWidget>(this, UnlockedUpgradeCardWidgetClass);
    check(UnlockedUpgradeCardWidget);

    UnlockedUpgradeCardWidget->UpgradeNameText->SetText(Upgrade.TextData.Name);
    UnlockedUpgradeCardWidget->UpgradeDescText->SetText(Upgrade.TextData.Description);

    UnlockedUpgradeCardWidget->UpgradeCostText->SetText(FText::FromString("Unlocked"));
    UnlockedUpgradeCardWidget->ReqsNotMetText->SetText(FText::FromString(""));

    TArray<FUpgradeEffect> UpgradeEffects = UpgradeManager->GetUpgradeEffectsByIds(Upgrade.UpgradeEffectIDs);
    TArray<FText> UpgradeEffectsTexts;
    for (const FUpgradeEffect& Effect : UpgradeEffects) UpgradeEffectsTexts.Add(Effect.TextData.Description);
    UnlockedUpgradeCardWidget->ReqsNotMetText->SetText(FText::Join(FText::FromString("\n"), UpgradeEffectsTexts));

    UnlockedUpgradesBox->AddChildToVerticalBox(UnlockedUpgradeCardWidget);
  }
}

void UPowerUpgradesWidget::InitUI(EUpgradeClass _UpgradeClass, class AUpgradeManager* _UpgradeManager) {
  check(_UpgradeManager);

  UpgradeClass = _UpgradeClass;
  UpgradeManager = _UpgradeManager;

  SelectedUpgradeID = FName();
  SelectedUpgradeCardWidget = nullptr;
}