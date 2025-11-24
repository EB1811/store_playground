#include "AbilityListWidget.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "store_playground/UI/Ability/AbilityCardWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UAbilityListWidget::SelectAbility(FName AbilityID) {
  SelectAbilityFunc(AbilityID);

  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);
}

void UAbilityListWidget::SelectHoveredAbility() {
  if (HoveredAbilityID.IsNone() || !HoveredAbilityCardWidget) return;

  SelectAbility(HoveredAbilityID);
}
void UAbilityListWidget::HoverNextAbility(FVector2D Direction) {
  if (Direction.X == 0) return;
  if (AvailableAbilityListBox->GetChildrenCount() <= 0) return;

  if (!HoveredAbilityCardWidget) {
    UAbilityCardWidget* AbilityCardWidget = Cast<UAbilityCardWidget>(AvailableAbilityListBox->GetChildAt(0));
    check(AbilityCardWidget);

    HoveredAbilityID = AbilityCardWidget->AbilityId;
    HoveredAbilityCardWidget = AbilityCardWidget;
    HoveredAbilityCardWidget->bIsHovered = true;
    HoveredAbilityCardWidget->RefreshUI();
    HoveredAbilityCardWidget->SelectButton->SetFocus();
    return;
  }

  int32 CurrentIndex = AvailableAbilityListBox->GetAllChildren().IndexOfByKey(HoveredAbilityCardWidget);
  check(CurrentIndex != INDEX_NONE);
  int32 NextIndex = CurrentIndex + (Direction.X > 0 ? 1 : -1);
  if (NextIndex < 0 || NextIndex >= AvailableAbilityListBox->GetChildrenCount()) return;

  HoveredAbilityCardWidget->bIsHovered = false;
  HoveredAbilityCardWidget->RefreshUI();

  UAbilityCardWidget* NextAbilityCardWidget = Cast<UAbilityCardWidget>(AvailableAbilityListBox->GetChildAt(NextIndex));
  HoveredAbilityID = NextAbilityCardWidget->AbilityId;
  HoveredAbilityCardWidget = NextAbilityCardWidget;
  HoveredAbilityCardWidget->bIsHovered = true;
  HoveredAbilityCardWidget->RefreshUI();
  HoveredAbilityCardWidget->SelectButton->SetFocus();
}

void UAbilityListWidget::RefreshUI() {
  AvailableAbilityListBox->ClearChildren();
  NotEnoughMoneyAbilityListBox->ClearChildren();
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

  if (AvailableAbilities.Num() > 0) {
    HoveredAbilityID = AvailableAbilities[0].ID;
    HoveredAbilityCardWidget = Cast<UAbilityCardWidget>(AvailableAbilityListBox->GetChildAt(0));
    check(HoveredAbilityCardWidget);
    HoveredAbilityCardWidget->bIsHovered = true;
    HoveredAbilityCardWidget->RefreshUI();
  }

  if (AvailableAbilities.Num() == 0 && NotEnoughMoneyAbilities.Num() == 0 && UnavailableAbilities.Num() == 0)
    NoAbilitiesText->SetVisibility(ESlateVisibility::Visible);
  else NoAbilitiesText->SetVisibility(ESlateVisibility::Collapsed);
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

  HoveredAbilityCardWidget = nullptr;
}