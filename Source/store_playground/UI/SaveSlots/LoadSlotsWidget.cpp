#include "LoadSlotsWidget.h"
#include "store_playground/UI/SaveSlots/PopulatedSaveSlotWidget.h"
#include "store_playground/UI/SaveSlots/EmptySaveSlotWidget.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "store_playground/Framework/StorePGGameMode.h"
#include "Kismet/GameplayStatics.h"

void ULoadSlotsWidget::RefreshUI() {
  check(SaveSlotListSaveGame && PopulatedSaveSlotWidgetClass && EmptySaveSlotWidgetClass && SelectSlotFunc);

  auto PopulatedSelectSlotFunc = [this](int32 SlotIndex) { SelectSlotFunc(SlotIndex); };
  auto EmptySelectSlotFunc = [this](int32 SlotIndex) { return; };

  SaveSlotsWrapBox->ClearChildren();
  for (int32 i = 0; i < SlotCount; i++) {
    if (i < SaveSlotListSaveGame->SaveSlotList.Num()) {
      FSaveSlotData SlotData = SaveSlotListSaveGame->SaveSlotList[i];
      if (UPopulatedSaveSlotWidget* PopulatedSaveSlotWidget =
              CreateWidget<UPopulatedSaveSlotWidget>(this, PopulatedSaveSlotWidgetClass)) {
        PopulatedSaveSlotWidget->SelectSlotFunc = PopulatedSelectSlotFunc;
        PopulatedSaveSlotWidget->SetSlotData(i, SlotData);

        SaveSlotsWrapBox->AddChildToWrapBox(PopulatedSaveSlotWidget);
      }
    } else if (UEmptySaveSlotWidget* EmptySaveSlotWidget =
                   CreateWidget<UEmptySaveSlotWidget>(this, EmptySaveSlotWidgetClass)) {
      EmptySaveSlotWidget->SelectSlotFunc = EmptySelectSlotFunc;
      EmptySaveSlotWidget->SlotIndex = i;

      EmptySaveSlotWidget->SelectButton->SetIsEnabled(false);

      SaveSlotsWrapBox->AddChildToWrapBox(EmptySaveSlotWidget);
    }
  }
}