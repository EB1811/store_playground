#include "AbilityListWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "store_playground/UI/Ability/AbilityCardWidget.h"

void UAbilityListWidget::RefreshUI() {
  check(AbilityCardWidgetClass);

  AbilityListPanelWrapBox->ClearChildren();

  for (const auto& Ability : EconEventAbilities) {
    if (UAbilityCardWidget* AbilityCardWidget = CreateWidget<UAbilityCardWidget>(GetWorld(), AbilityCardWidgetClass)) {
      UE_LOG(LogTemp, Warning, TEXT("Ability ID: %s"), *Ability.TextData.Name.ToString());
      AbilityCardWidget->SelectAbilityFunc = SelectAbilityFunc;

      AbilityCardWidget->AbilityId = Ability.ID;
      AbilityCardWidget->AbilityIconImage->SetBrushFromTexture(Ability.AssetData.Icon);
      AbilityCardWidget->AbilityNameText->SetText(Ability.TextData.Name);
      AbilityCardWidget->AbilityDescText->SetText(Ability.TextData.Description);
      AbilityCardWidget->AbilityCostText->SetText(FText::AsNumber(Ability.Cost));

      switch (Ability.UpgradeClass) {
        case EUpgradeClass::Holy:
          AbilityCardWidget->AbilityIconImage->SetColorAndOpacity(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f));
          break;
        case EUpgradeClass::Demonic:
          AbilityCardWidget->AbilityIconImage->SetColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
          break;
        case EUpgradeClass::Artisanal:
          AbilityCardWidget->AbilityIconImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 0.0f, 1.0f));
          break;
      }

      AbilityListPanelWrapBox->AddChildToWrapBox(AbilityCardWidget);
    }
  }
}
