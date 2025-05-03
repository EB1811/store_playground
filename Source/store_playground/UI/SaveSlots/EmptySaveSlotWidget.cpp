#include "EmptySaveSlotWidget.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UEmptySaveSlotWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SelectButton->OnClicked.AddDynamic(this, &UEmptySaveSlotWidget::OnSelectButtonClicked);
}

void UEmptySaveSlotWidget::OnSelectButtonClicked() {
  check(SelectSlotFunc);

  SelectSlotFunc(SlotIndex);
}