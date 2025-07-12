#include "PopulatedSaveSlotWidget.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UPopulatedSaveSlotWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SelectButton->OnClicked.AddDynamic(this, &UPopulatedSaveSlotWidget::OnSelectButtonClicked);
}

void UPopulatedSaveSlotWidget::SetSlotData(int32 index, const FSaveSlotData& SlotData) {
  SlotIndex = index;
  NameText->SetText(FText::FromString(SlotData.SlotName));
  LastModifiedText->SetText(FText::FromString(SlotData.LastModified.ToFormattedString(*FString("%d/%m/%y"))));
  CurrentDayText->SetText(FText::FromString(FString::Printf(TEXT("Day: %d"), SlotData.CurrentDay)));
  StoreMoneyText->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), SlotData.StoreMoney)));
}

void UPopulatedSaveSlotWidget::OnSelectButtonClicked() {
  check(SelectSlotFunc);

  SelectSlotFunc(SlotIndex, this);
}