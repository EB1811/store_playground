#include "StoreExpansionSelectWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UStoreExpansionSelectWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SelectButton->OnClicked.AddDynamic(this, &UStoreExpansionSelectWidget::OnSelectButtonClicked);
}

void UStoreExpansionSelectWidget::SetExpansionData(const FStoreExpansionData& Expansion) {
  StoreExpansionLevel = Expansion.StoreExpansionLevel;

  NameText->SetText(FText::FromName(Expansion.Name));
  PriceText->SetText(FText::FromString(FString::SanitizeFloat((FMath::RoundToInt32(Expansion.Price)))));
  IsLockedText->SetText(Expansion.bIsLocked ? FText::FromString("Locked") : FText::FromString(""));
  IsLockedText->SetVisibility(Expansion.bIsLocked ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
  SelectButton->SetIsEnabled(!Expansion.bIsLocked);
}

void UStoreExpansionSelectWidget::OnSelectButtonClicked() {
  check(SelectExpansionFunc);

  SelectExpansionFunc(StoreExpansionLevel);
}
