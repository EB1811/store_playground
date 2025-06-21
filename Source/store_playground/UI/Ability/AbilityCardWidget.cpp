#include "AbilityCardWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UAbilityCardWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SelectButton->OnClicked.AddDynamic(this, &UAbilityCardWidget::OnSelectButtonClicked);
}

void UAbilityCardWidget::InitUI(FEconEventAbility Ability, std::function<void(FName)> _SelectAbilityFunc) {
  check(_SelectAbilityFunc);

  AbilityId = Ability.ID;
  SelectAbilityFunc = _SelectAbilityFunc;

  NameText->SetText(Ability.TextData.Name);
  DescText->SetText(Ability.TextData.Description);
  CostText->SetText(FText::FromString(FString::Printf(TEXT("Cost: %.0f¬"), Ability.Cost)));
  CooldownText->SetText(FText::FromString(FString::Printf(TEXT("Cooldown: %d days"), Ability.Cooldown)));
}

void UAbilityCardWidget::OnSelectButtonClicked() {
  check(SelectAbilityFunc);

  SelectAbilityFunc(AbilityId);
}
