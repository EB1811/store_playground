#include "AbilityListWidget.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "store_playground/UI/Ability/AbilityCardWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"

void UAbilityListWidget::RefreshUI() {
  AvailableAbilityListBox->ClearChildren();
  UnavailableAbilityListBox->ClearChildren();

  for (const FEconEventAbility& Ability : AvailableAbilities) {
    UAbilityCardWidget* AbilityCardWidget = CreateWidget<UAbilityCardWidget>(this, AbilityCardWidgetClass);
    check(AbilityCardWidget);

    AbilityCardWidget->InitUI(Ability, SelectAbilityFunc);
    AvailableAbilityListBox->AddChild(AbilityCardWidget);
  }
  for (const FEconEventAbility& Ability : NotEnoughMoneyAbilities) {
    UAbilityCardWidget* AbilityCardWidget = CreateWidget<UAbilityCardWidget>(this, AbilityCardWidgetClass);
    check(AbilityCardWidget);

    AbilityCardWidget->InitUI(Ability, SelectAbilityFunc);
    AbilityCardWidget->SelectButton->SetIsEnabled(false);
    AbilityCardWidget->CostText->SetColorAndOpacity(FColor::FromHex("B4494BFF"));
    NotEnoughMoneyAbilityListBox->AddChild(AbilityCardWidget);
  }
  for (const FEconEventAbility& Ability : UnavailableAbilities) {
    UAbilityCardWidget* AbilityCardWidget = CreateWidget<UAbilityCardWidget>(this, AbilityCardWidgetClass);
    check(AbilityCardWidget);

    AbilityCardWidget->InitUI(Ability, SelectAbilityFunc);
    AbilityCardWidget->SelectButton->SetIsEnabled(false);
    AbilityCardWidget->CooldownText->SetColorAndOpacity(FColor::FromHex("B4494BFF"));
    UnavailableAbilityListBox->AddChild(AbilityCardWidget);
  }
}

void UAbilityListWidget::InitUI(TArray<FEconEventAbility> _AvailableAbilities,
                                TArray<FEconEventAbility> _NotEnoughMoneyAbilities,
                                TArray<FEconEventAbility> _UnavailableAbilities,
                                std::function<void(FName)> _SelectAbilityFunc) {
  check(_SelectAbilityFunc && AbilityCardWidgetClass);

  AvailableAbilities = _AvailableAbilities;
  NotEnoughMoneyAbilities = _NotEnoughMoneyAbilities;
  UnavailableAbilities = _UnavailableAbilities;
  SelectAbilityFunc = _SelectAbilityFunc;
}