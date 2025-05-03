#include "SaveSlotsWidget.h"
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

void USaveSlotsWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ExitMenuButton->OnClicked.AddDynamic(this, &USaveSlotsWidget::OnExitMenuButtonClicked);
}

void USaveSlotsWidget::RefreshUI() {
  check(SaveManagerRef && PopulatedSaveSlotWidgetClass && EmptySaveSlotWidgetClass);

  auto PopulatedSelectSlotFunc = [this](int32 SlotIndex) {
    if (bIsSaving) {
      SaveManagerRef->CreateNewSaveGame(SlotIndex);

      RefreshUI();
    } else {
      AStorePGGameMode* GameMode = Cast<AStorePGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
      check(GameMode);

      GameMode->LoadGame(SlotIndex);
    }
  };
  auto EmptySelectSlotFunc = [this](int32 SlotIndex) {
    if (bIsSaving) SaveManagerRef->CreateNewSaveGame(SlotIndex);

    RefreshUI();
  };

  SaveSlotsWrapBox->ClearChildren();
  int32 SlotCount = SaveManagerRef->SaveManagerParams.SaveSlotCount;
  for (int32 i = 0; i < SlotCount; i++) {
    if (i < SaveManagerRef->SaveSlotListSaveGame->SaveSlotList.Num()) {
      FSaveSlotData SlotData = SaveManagerRef->SaveSlotListSaveGame->SaveSlotList[i];
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

      if (bIsSaving) EmptySaveSlotWidget->SelectButton->SetIsEnabled(true);
      else EmptySaveSlotWidget->SelectButton->SetIsEnabled(false);

      SaveSlotsWrapBox->AddChildToWrapBox(EmptySaveSlotWidget);
    }
  }
}

void USaveSlotsWidget::OnExitMenuButtonClicked() { CloseSaveSlotsUI(); }