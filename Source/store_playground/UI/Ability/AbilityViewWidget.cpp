#include "AbilityViewWidget.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Components/ControlTextWidget.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "store_playground/UI/Ability/AbilitySelectWidget.h"
#include "store_playground/UI/Ability/AbilityListWidget.h"
#include "store_playground/Upgrade/UpgradeSelectComponent.h"
#include "store_playground/Ability/AbilityManager.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UAbilityViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ActivateButton->ControlButton->OnClicked.AddDynamic(this, &UAbilityViewWidget::Activate);
  SelectReplaceAbilityButton->ControlButton->OnClicked.AddDynamic(this, &UAbilityViewWidget::SelectReplaceAbility);
  BackButton->ControlButton->OnClicked.AddDynamic(this, &UAbilityViewWidget::Back);
}

void UAbilityViewWidget::Activate() {
  if (SelectedAbilityID.IsNone()) return;

  AbilityManager->ActivateEconEventAbility(SelectedAbilityID);

  SelectedAbilityID = FName();
  RefreshUI();
}

void UAbilityViewWidget::SelectReplaceAbility() {
  if (bShowingAbilitySelect) {
    bShowingAbilitySelect = false;
    AbilityListWidget->SetVisibility(ESlateVisibility::Collapsed);
  } else {
    bShowingAbilitySelect = true;

    TArray<FEconEventAbility> AvailableAbilities = AbilityManager->GetAvailableEconEventAbilities();
    TArray<FEconEventAbility> NotEnoughMoneyAbilities = AbilityManager->GetNotEnoughMoneyEconEventAbilities();
    TArray<FEconEventAbility> UnavailableAbilities = AbilityManager->GetCooldownEconEventAbilities();
    AbilityListWidget->InitUI(AvailableAbilities, NotEnoughMoneyAbilities, UnavailableAbilities,
                              [this](FName AbilityID) {
                                SelectedAbilityID = AbilityID;
                                SelectReplaceAbility();
                              });
    AbilityListWidget->SetVisibility(ESlateVisibility::Visible);
  }
  RefreshUI();
}

void UAbilityViewWidget::Back() {
  if (bShowingAbilitySelect) {
    bShowingAbilitySelect = false;
    AbilityListWidget->SetVisibility(ESlateVisibility::Collapsed);
    return;
  }

  CloseWidgetFunc();
}

void UAbilityViewWidget::RefreshUI() {
  if (bShowingAbilitySelect) AbilityListWidget->RefreshUI();

  if (SelectedAbilityID.IsNone()) {
    AbilitySelectWidget->Name->SetText(FText::FromString("No Ability Selected"));
    AbilitySelectWidget->CostText->SetText(FText::FromString(""));
    AbilitySelectWidget->CooldownText->SetText(FText::FromString(""));
    return;
  }

  FEconEventAbility SelectedAbility = AbilityManager->GetAbilityById(SelectedAbilityID);
  AbilitySelectWidget->Name->SetText(SelectedAbility.TextData.Name);
  AbilitySelectWidget->CostText->SetText(FText::FromString(FString::Printf(TEXT("Cost: %d¬"), SelectedAbility.Cost)));
  AbilitySelectWidget->CooldownText->SetText(
      FText::FromString(FString::Printf(TEXT("Cooldown: %d days"), SelectedAbility.Cooldown)));
}

void UAbilityViewWidget::InitUI(FInputActions InputActions,
                                class AAbilityManager* _AbilityManager,
                                std::function<void()> _CloseWidgetFunc) {
  check(_AbilityManager && _CloseWidgetFunc);

  AbilityListWidget->SetVisibility(ESlateVisibility::Collapsed);

  AbilityManager = _AbilityManager;

  SelectedAbilityID = FName();
  bShowingAbilitySelect = false;

  ActivateButton->ActionText->SetText(FText::FromString("Activate"));
  SelectReplaceAbilityButton->ActionText->SetText(FText::FromString("Select/Replace Ability"));
  BackButton->ActionText->SetText(FText::FromString("Back"));

  CloseWidgetFunc = _CloseWidgetFunc;
}