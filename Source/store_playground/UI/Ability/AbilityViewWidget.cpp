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
#include "store_playground/Store/Store.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UAbilityViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ActivateButton->ControlButton->OnClicked.AddDynamic(this, &UAbilityViewWidget::Activate);
  SelectReplaceAbilityButton->ControlButton->OnClicked.AddDynamic(this, &UAbilityViewWidget::SelectReplaceAbility);
  BackButton->ControlButton->OnClicked.AddDynamic(this, &UAbilityViewWidget::Back);

  SetupUIActionable();
  SetupUIBehaviour();
}

void UAbilityViewWidget::Activate() {
  if (SelectedAbilityID.IsNone()) return;

  FEconEventAbility SelectedAbility = AbilityManager->GetAbilityById(SelectedAbilityID);
  check(SelectedAbility.Cost <= Store->GetAvailableMoney());  // SelectReplaceAbility should ensure this.

  AbilityManager->ActivateEconEventAbility(SelectedAbilityID);

  SelectedAbilityID = FName();
  RefreshUI();

  UGameplayStatics::PlaySound2D(this, ActivateSound, 1.0f);
}

void UAbilityViewWidget::SelectReplaceAbility() {
  if (bShowingAbilitySelect) {
    bShowingAbilitySelect = false;
    AbilityListWidget->SetVisibility(ESlateVisibility::Collapsed);

    ActivateButton->SetVisibility(ESlateVisibility::Visible);
    SelectReplaceAbilityButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton4Action);
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

    ActivateButton->SetVisibility(ESlateVisibility::Collapsed);
    SelectReplaceAbilityButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.AdvanceUIAction);
  }
  RefreshUI();

  UGameplayStatics::PlaySound2D(this, SelectReplaceSound, 1.0f);
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
  AbilitySelectWidget->CostText->SetText(FText::FromString(FString::Printf(TEXT("Cost: %.0f¬"), SelectedAbility.Cost)));
  AbilitySelectWidget->CooldownText->SetText(
      FText::FromString(FString::Printf(TEXT("Cooldown: %d days"), SelectedAbility.Cooldown)));
}

void UAbilityViewWidget::InitUI(FInUIInputActions _InUIInputActions,
                                const class AStore* _Store,
                                class AAbilityManager* _AbilityManager,
                                std::function<void()> _CloseWidgetFunc) {
  check(_Store && _AbilityManager && _CloseWidgetFunc);

  AbilityListWidget->SetVisibility(ESlateVisibility::Collapsed);

  InUIInputActions = _InUIInputActions;
  Store = _Store;
  AbilityManager = _AbilityManager;

  SelectedAbilityID = FName();
  bShowingAbilitySelect = false;

  ActivateButton->ActionText->SetText(FText::FromString("Activate"));
  ActivateButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.AdvanceUIAction);
  SelectReplaceAbilityButton->ActionText->SetText(FText::FromString("Select/Replace Ability"));
  SelectReplaceAbilityButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton4Action);
  BackButton->ActionText->SetText(FText::FromString("Back"));
  BackButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.RetractUIAction);

  CloseWidgetFunc = _CloseWidgetFunc;
}

void UAbilityViewWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() {
    if (bShowingAbilitySelect) AbilityListWidget->SelectHoveredAbility();
    else Activate();
  };
  UIActionable.DirectionalInput = [this](FVector2D Direction) {
    if (bShowingAbilitySelect) AbilityListWidget->HoverNextAbility(Direction);
  };
  UIActionable.SideButton4 = [this]() {
    if (bShowingAbilitySelect) return;
    else SelectReplaceAbility();
  };
  UIActionable.RetractUI = [this]() {
    if (bShowingAbilitySelect) SelectReplaceAbility();
    else Back();
  };
  UIActionable.QuitUI = [this]() { CloseWidgetFunc(); };
}

void UAbilityViewWidget::SetupUIBehaviour() {
  UIBehaviour.ShowAnim = ShowAnim;
  UIBehaviour.HideAnim = HideAnim;
  UIBehaviour.OpenSound = OpenSound;
  UIBehaviour.HideSound = HideSound;
}