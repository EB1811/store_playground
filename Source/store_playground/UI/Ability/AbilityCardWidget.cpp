#include "AbilityCardWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UAbilityCardWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ActivateAbilityButton->OnClicked.AddDynamic(this, &UAbilityCardWidget::OnActivateAbilityButtonClicked);
}

void UAbilityCardWidget::OnActivateAbilityButtonClicked() {
  check(SelectAbilityFunc);

  SelectAbilityFunc(AbilityId);
}
