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
  LastModifiedText->SetText(FText::FromString(SlotData.LastModified.ToString()));
  CurrentDayText->SetText(FText::FromString(FString::FromInt(SlotData.CurrentDay)));
  StoreMoneyText->SetText(FText::FromString(FString::FromInt(SlotData.StoreMoney)));
}

void UPopulatedSaveSlotWidget::OnSelectButtonClicked() {
  check(SelectSlotFunc);

  SelectSlotFunc(SlotIndex);
}