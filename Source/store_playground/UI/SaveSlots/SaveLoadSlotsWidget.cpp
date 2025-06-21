#include "SaveLoadSlotsWidget.h"
#include "store_playground/UI/SaveSlots/PopulatedSaveSlotWidget.h"
#include "store_playground/UI/SaveSlots/EmptySaveSlotWidget.h"
#include "store_playground/UI/SaveSlots/SaveLoadConfirmWidget.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "store_playground/Framework/StorePGGameMode.h"
#include "Kismet/GameplayStatics.h"

void USaveLoadSlotsWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SelectButton->ControlButton->OnClicked.AddDynamic(this, &USaveLoadSlotsWidget::Select);
  DeleteButton->ControlButton->OnClicked.AddDynamic(this, &USaveLoadSlotsWidget::Delete);
  BackButton->ControlButton->OnClicked.AddDynamic(this, &USaveLoadSlotsWidget::Back);
}

void USaveLoadSlotsWidget::ConfirmSaveLoad(int32 SlotIndex) {
  check(SaveManagerRef);
  if (!bIsSaving) check(SlotIndex < SaveManagerRef->SaveManagerParams.SaveSlotCount);

  if (bIsConfirming) return;

  FText Text = FText::Format(
      NSLOCTEXT("SaveLoadSlotsWidget", "ConfirmSaveLoad", "Are you sure you want to {0} slot {1}?"),
      bIsSaving ? NSLOCTEXT("SaveLoadSlotsWidget", "Save", "save") : NSLOCTEXT("SaveLoadSlotsWidget", "Load", "load"),
      FText::AsNumber(SlotIndex + 1));
  auto ConfirmFunc = [this, SlotIndex]() {
    if (bIsSaving) {
      SaveManagerRef->CreateNewSaveGame(SlotIndex);

      SaveLoadConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);
      bIsConfirming = false;

      RefreshUI();
    } else {
      AStorePGGameMode* GameMode = Cast<AStorePGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
      check(GameMode);

      GameMode->LoadGame(SlotIndex);
    }
  };
  auto CancelFunc = [this]() {
    SaveLoadConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);
    bIsConfirming = false;
  };
  SaveLoadConfirmWidget->InitUI(Text, ConfirmFunc, CancelFunc);

  SaveLoadConfirmWidget->SetVisibility(ESlateVisibility::Visible);
  bIsConfirming = true;
}
void USaveLoadSlotsWidget::ConfirmDelete(int32 SlotIndex) {
  check(SaveManagerRef);

  if (bIsConfirming) return;

  FText Text =
      FText::Format(NSLOCTEXT("SaveLoadSlotsWidget", "ConfirmDelete", "Are you sure you want to delete slot {0}?"),
                    FText::AsNumber(SlotIndex + 1));
  auto ConfirmFunc = [this, SlotIndex]() {
    // SaveManagerRef->DeleteSaveGame(SlotIndex); // TODO
    RefreshUI();
  };
  auto CancelFunc = [this]() {
    SaveLoadConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);
    bIsConfirming = false;
  };
  SaveLoadConfirmWidget->InitUI(Text, ConfirmFunc, CancelFunc);

  SaveLoadConfirmWidget->SetVisibility(ESlateVisibility::Visible);
  bIsConfirming = true;
}

void USaveLoadSlotsWidget::Select() {
  // TODO:
  // if (bIsConfirming) return;

  // int32 SlotIndex = -1;
  // if (UEmptySaveSlotWidget* EmptySaveSlotWidget = Cast<UEmptySaveSlotWidget>(GetFocusedWidget()))
  //   SlotIndex = EmptySaveSlotWidget->SlotIndex;
  // else if (UPopulatedSaveSlotWidget* PopulatedSaveSlotWidget = Cast<UPopulatedSaveSlotWidget>(GetFocusedWidget()))
  //   SlotIndex = PopulatedSaveSlotWidget->SlotIndex;

  // if (SlotIndex < 0) return;

  // ConfirmSaveLoad(SlotIndex);
}
void USaveLoadSlotsWidget::Delete() {
  // TODO:
  // if (bIsConfirming) return;

  // int32 SlotIndex = -1;
  // if (UEmptySaveSlotWidget* EmptySaveSlotWidget = Cast<UEmptySaveSlotWidget>(GetFocusedWidget()))
  //   SlotIndex = EmptySaveSlotWidget->SlotIndex;
  // else if (UPopulatedSaveSlotWidget* PopulatedSaveSlotWidget = Cast<UPopulatedSaveSlotWidget>(GetFocusedWidget()))
  //   SlotIndex = PopulatedSaveSlotWidget->SlotIndex;

  // if (SlotIndex < 0) return;

  // ConfirmDelete(SlotIndex);
}
void USaveLoadSlotsWidget::Back() {
  if (bIsConfirming) {
    SaveLoadConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);
    bIsConfirming = false;
  } else {
    BackFunc();
  }
}

void USaveLoadSlotsWidget::RefreshUI() {
  check(SaveManagerRef && PopulatedSaveSlotWidgetClass && EmptySaveSlotWidgetClass);

  auto PopulatedSelectSlotFunc = [this](int32 SlotIndex) { ConfirmSaveLoad(SlotIndex); };
  auto EmptySelectSlotFunc = [this](int32 SlotIndex) {
    if (bIsSaving) ConfirmSaveLoad(SlotIndex);
  };

  SlotsBox->ClearChildren();
  int32 SlotCount = SaveManagerRef->SaveManagerParams.SaveSlotCount;
  for (int32 i = 0; i < SlotCount; i++) {
    if (i < SaveManagerRef->SaveSlotListSaveGame->SaveSlotList.Num()) {
      FSaveSlotData SlotData = SaveManagerRef->SaveSlotListSaveGame->SaveSlotList[i];
      if (UPopulatedSaveSlotWidget* PopulatedSaveSlotWidget =
              CreateWidget<UPopulatedSaveSlotWidget>(this, PopulatedSaveSlotWidgetClass)) {
        PopulatedSaveSlotWidget->SelectSlotFunc = PopulatedSelectSlotFunc;
        PopulatedSaveSlotWidget->SetSlotData(i, SlotData);

        SlotsBox->AddChildToVerticalBox(PopulatedSaveSlotWidget);
      }
    } else if (UEmptySaveSlotWidget* EmptySaveSlotWidget =
                   CreateWidget<UEmptySaveSlotWidget>(this, EmptySaveSlotWidgetClass)) {
      EmptySaveSlotWidget->SelectSlotFunc = EmptySelectSlotFunc;
      EmptySaveSlotWidget->SlotIndex = i;

      if (bIsSaving) EmptySaveSlotWidget->SelectButton->SetIsEnabled(true);
      else EmptySaveSlotWidget->SelectButton->SetIsEnabled(false);

      SlotsBox->AddChildToVerticalBox(EmptySaveSlotWidget);
    }
  }
}

void USaveLoadSlotsWidget::InitUI(bool _bIsSaving,
                                  class ASaveManager* _SaveManager,
                                  std::function<void()> _CloseWidgetFunc) {
  check(_SaveManager && _CloseWidgetFunc);

  bIsSaving = _bIsSaving;
  SaveManagerRef = _SaveManager;
  BackFunc = _CloseWidgetFunc;

  if (bIsSaving) TitleText->SetText(NSLOCTEXT("SaveLoadSlotsWidget", "SaveTitle", "Save"));
  else TitleText->SetText(NSLOCTEXT("SaveLoadSlotsWidget", "LoadTitle", "Load"));

  SelectButton->ActionText->SetText(FText::FromString("Select"));
  DeleteButton->ActionText->SetText(FText::FromString("Delete"));
  BackButton->ActionText->SetText(FText::FromString("Back"));

  bIsConfirming = false;
  SaveLoadConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);
}