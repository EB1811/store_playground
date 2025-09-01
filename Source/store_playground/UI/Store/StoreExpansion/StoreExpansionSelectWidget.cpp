#include "StoreExpansionSelectWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UStoreExpansionSelectWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SelectButton->OnClicked.AddDynamic(this, &UStoreExpansionSelectWidget::Select);
}

void UStoreExpansionSelectWidget::Select() { SelectFunc(ExpansionID); }

void UStoreExpansionSelectWidget::RefreshUI() {
  if (bIsSelected) SelectButton->SetIsEnabled(false);
  else SelectButton->SetIsEnabled(true);
}

void UStoreExpansionSelectWidget::InitUI(const FStoreExpansionData& ExpansionData,
                                         std::function<void(FName)> _SelectUpgradeFunc) {
  check(_SelectUpgradeFunc);

  ExpansionID = ExpansionData.StoreExpansionLevelID;
  SelectFunc = _SelectUpgradeFunc;

  NameText->SetText(ExpansionData.Name);
  PictureImage->SetBrushFromMaterial(ExpansionData.Picture);
  PriceText->SetText(FText::FromString(FString::Printf(TEXT("Cost: %.0f¬"), ExpansionData.Price)));
}