#include "AbilityWidget.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "store_playground/UI/Ability/AbilityListWidget.h"
#include "store_playground/Ability/AbilityManager.h"

void UAbilityWidget::RefreshUI() {
  check(AbilityManagerRef);

  const auto& AvailableEconEventAbilities = AbilityManagerRef->GetAvailableEconEventAbilities();
  AbilityListWidget->EconEventAbilities = AvailableEconEventAbilities;

  AbilityListWidget->SelectAbilityFunc = [this](FName AbilityId) {
    AbilityManagerRef->ActivateEconEventAbility(AbilityId);
    return RefreshUI();
  };

  AbilityListWidget->RefreshUI();
}
