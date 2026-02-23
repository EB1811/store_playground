#include "DebtWidget.h"
#include "Components/Slider.h"
#include "Logging/LogVerbosity.h"
#include "TimerManager.h"
#include "store_playground/DebtManager/DebtManager.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UDebtWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  DebtAmountSlider->OnValueChanged.AddDynamic(this, &UDebtWidget::UpdateDebtTexts);
  IssueDebtButton->ControlButton->OnClicked.AddDynamic(this, &UDebtWidget::IssueDebt);
}

void UDebtWidget::UpdateDebtTexts(float Value) {
  float Interest = DebtManager->GetInterest(Value);

  DebtAmountText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), Value)));
  PayableInterestText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), Interest)));
  TotalPayableText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), Value + Interest)));
}

void UDebtWidget::ChangeAmount(FVector2D Direction) {
  if (Direction.Y == 0) return;
  if (DebtAmountSlider->GetMaxValue() <= DebtAmountSlider->GetStepSize()) return;

  float NewValue = FMath::Clamp(DebtAmountSlider->GetValue() + Direction.Y * DebtAmountSlider->GetStepSize(),
                                DebtAmountSlider->GetMinValue(), DebtAmountSlider->GetMaxValue());
  if (NewValue != DebtAmountSlider->GetValue()) UGameplayStatics::PlaySound2D(this, ChangeAmountSound, 1.0f);

  DebtAmountSlider->SetValue(NewValue);
  UpdateDebtTexts(NewValue);
}

void UDebtWidget::IssueDebt() {
  float AmountToIssue = DebtAmountSlider->GetValue();
  if (AmountToIssue <= 0) return;

  if (AmountToIssue > DebtManager->GetMaxDebt()) {
    UE_LOG(LogTemp, Warning, TEXT("Cannot issue debt. Amount: %.0f, Max Debt: %.0f"), AmountToIssue,
           DebtManager->GetMaxDebt());
    return;
  }

  DebtManager->IssueDebt(AmountToIssue);
  RefreshUI();

  UGameplayStatics::PlaySound2D(this, IssueDebtSound, 1.0f);
}

void UDebtWidget::ResetAmount() {
  DebtAmountSlider->SetValue(0.0f);
  DebtAmountSlider->SetMaxValue(DebtManager->GetMaxDebt());

  UpdateDebtTexts(0.0f);
}

void UDebtWidget::RefreshUI() {
  DebtTakenText->SetText(
      FText::FromString(FString::Printf(TEXT("%.0f¬"), DebtManager->AdditionalDebt - DebtManager->InterestAccrued)));
  CurrentOwnedDebtText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), DebtManager->GetPayableDebt())));
  InterestRateText->SetText(
      FText::FromString(FString::Printf(TEXT("%.1f"), DebtManager->GetInterestRate() * 100) + TEXT("%")));
  MaxDebtText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), DebtManager->GetDebtLimit())));

  DebtAmountSlider->SetValue(0.0f);
  DebtAmountSlider->SetMaxValue(DebtManager->GetMaxDebt());
  UpdateDebtTexts(0.0f);

  if (DebtAmountSlider->GetMaxValue() <= DebtAmountSlider->GetStepSize())
    IssueDebtButton->ControlButton->SetIsEnabled(false);
  else IssueDebtButton->ControlButton->SetIsEnabled(true);
}
void UDebtWidget::InitUI(FInUIInputActions InUIInputActions,
                         const class AStore* _Store,
                         class ADebtManager* _DebtManager) {
  check(_DebtManager && _Store);

  Store = _Store;
  DebtManager = _DebtManager;

  IssueDebtButton->ActionText->SetText(FText::FromString("Issue"));
  IssueDebtButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.AdvanceUIAction);
}