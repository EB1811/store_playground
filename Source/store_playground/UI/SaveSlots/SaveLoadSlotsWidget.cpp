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

  SetupUIActionable();
}

void USaveLoadSlotsWidget::ConfirmSaveLoad(int32 SlotIndex) {
  check(SaveManagerRef);
  if (!bIsSaving) check(SlotIndex < SaveManagerRef->SaveManagerParams.SaveSlotCount);
  if (bIsSaving) check(SaveManagerRef->CanSave());

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
    SaveManagerRef->DeleteSaveGame(SlotIndex);

    SaveLoadConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);
    bIsConfirming = false;

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
  if (bIsConfirming) return;
  if (SelectedSlotIndex < 0) return;

  ConfirmSaveLoad(SelectedSlotIndex);
}
void USaveLoadSlotsWidget::Delete() {
  if (bIsConfirming) return;
  if (SelectedSlotIndex < 0) return;
  if (SelectedSlotIndex >= SaveManagerRef->SaveSlotListSaveGame->SaveSlotList.Num()) return;

  ConfirmDelete(SelectedSlotIndex);
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

  auto SelectSlotFunc = [this](int32 SlotIndex, UUserWidget* Widget) {
    if (bIsConfirming) return;
    if (SelectedSlotIndex == SlotIndex) return;

    FSaveSlotData SlotData = SaveManagerRef->SaveSlotListSaveGame->SaveSlotList[SlotIndex];
    if (SlotData.bIsPopulated) {
      UPopulatedSaveSlotWidget* NewSelectedPopulated = Cast<UPopulatedSaveSlotWidget>(Widget);
      check(NewSelectedPopulated);

      NewSelectedPopulated->SelectButton->SetIsEnabled(false);
    } else {
      UEmptySaveSlotWidget* NewSelectedEmpty = Cast<UEmptySaveSlotWidget>(Widget);
      check(NewSelectedEmpty);

      NewSelectedEmpty->SelectButton->SetIsEnabled(false);
    }
    if (SelectedSlotIndex >= 0) {
      FSaveSlotData OldSlotData = SaveManagerRef->SaveSlotListSaveGame->SaveSlotList[SelectedSlotIndex];
      if (OldSlotData.bIsPopulated) {
        UPopulatedSaveSlotWidget* OldSelectedPopulated = Cast<UPopulatedSaveSlotWidget>(SelectedSlotWidget);
        check(OldSelectedPopulated);

        OldSelectedPopulated->SelectButton->SetIsEnabled(true);
      } else {
        UEmptySaveSlotWidget* OldSelectedEmpty = Cast<UEmptySaveSlotWidget>(SelectedSlotWidget);
        check(OldSelectedEmpty);

        OldSelectedEmpty->SelectButton->SetIsEnabled(true);
      }
    }

    SelectedSlotIndex = SlotIndex;
    SelectedSlotWidget = Widget;
  };

  SlotsBox->ClearChildren();
  SelectedSlotIndex = -1;
  SelectedSlotWidget = nullptr;

  int32 SlotCount = SaveManagerRef->SaveManagerParams.SaveSlotCount;
  for (int32 i = 0; i < SlotCount; i++) {
    FSaveSlotData SlotData = SaveManagerRef->SaveSlotListSaveGame->SaveSlotList[i];
    if (SlotData.bIsPopulated) {
      if (UPopulatedSaveSlotWidget* PopulatedSaveSlotWidget =
              CreateWidget<UPopulatedSaveSlotWidget>(this, PopulatedSaveSlotWidgetClass)) {
        PopulatedSaveSlotWidget->SelectSlotFunc = SelectSlotFunc;
        PopulatedSaveSlotWidget->SetSlotData(i, SlotData);

        SlotsBox->AddChildToVerticalBox(PopulatedSaveSlotWidget);
      }
    } else if (UEmptySaveSlotWidget* EmptySaveSlotWidget =
                   CreateWidget<UEmptySaveSlotWidget>(this, EmptySaveSlotWidgetClass)) {
      EmptySaveSlotWidget->SelectSlotFunc = SelectSlotFunc;
      EmptySaveSlotWidget->SlotIndex = i;

      if (bIsSaving) EmptySaveSlotWidget->SelectButton->SetIsEnabled(true);
      else EmptySaveSlotWidget->SelectButton->SetIsEnabled(false);

      SlotsBox->AddChildToVerticalBox(EmptySaveSlotWidget);
    }
  }

  // if (SlotsBox->GetChildrenCount() == 0) return;
  // SelectedSlotIndex = 0;
  // SelectedSlotWidget = SlotsBox->GetChildAt(0);
}

void USaveLoadSlotsWidget::InitUI(FInUIInputActions InUIInputActions,
                                  bool _bIsSaving,
                                  class ASaveManager* _SaveManager,
                                  std::function<void()> _CloseWidgetFunc) {
  check(_SaveManager && _CloseWidgetFunc);

  bIsSaving = _bIsSaving;
  SaveManagerRef = _SaveManager;
  BackFunc = _CloseWidgetFunc;

  if (bIsSaving) TitleText->SetText(NSLOCTEXT("SaveLoadSlotsWidget", "SaveTitle", "Save"));
  else TitleText->SetText(NSLOCTEXT("SaveLoadSlotsWidget", "LoadTitle", "Load"));

  SelectButton->ActionText->SetText(FText::FromString("Select"));
  SelectButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.AdvanceUIAction);
  DeleteButton->ActionText->SetText(FText::FromString("Delete"));
  DeleteButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton4Action);
  BackButton->ActionText->SetText(FText::FromString("Back"));
  BackButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.RetractUIAction);

  SaveLoadConfirmWidget->ConfirmButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.AdvanceUIAction);
  SaveLoadConfirmWidget->CancelButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.RetractUIAction);

  bIsConfirming = false;
  SaveLoadConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void USaveLoadSlotsWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() {
    if (bIsConfirming) return SaveLoadConfirmWidget->Confirm();
    Select();
  };
  UIActionable.DirectionalInput = [this](FVector2D Direction) {};
  UIActionable.SideButton4 = [this]() {
    if (bIsConfirming) return SaveLoadConfirmWidget->Confirm();
    Delete();
  };
  UIActionable.RetractUI = [this]() {
    if (bIsConfirming) return SaveLoadConfirmWidget->Cancel();
    Back();
  };
}