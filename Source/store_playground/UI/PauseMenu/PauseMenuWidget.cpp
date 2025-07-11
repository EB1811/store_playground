#include "PauseMenuWidget.h"
#include "store_playground/UI/SaveSlots/SaveSlotsWidget.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UPauseMenuWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SaveButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnSaveButtonClicked);
  LoadButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnLoadButtonClicked);
  // QuitButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitButtonClicked);

  SaveSlotsWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UPauseMenuWidget::OnSaveButtonClicked() {
  if (!SaveManagerRef->CanSave()) return;

  SaveSlotsWidget->SetVisibility(ESlateVisibility::Visible);

  SaveSlotsWidget->SaveManagerRef = SaveManagerRef;
  SaveSlotsWidget->bIsSaving = true;
  SaveSlotsWidget->CloseSaveSlotsUI = [this]() { SaveSlotsWidget->SetVisibility(ESlateVisibility::Collapsed); };

  SaveSlotsWidget->RefreshUI();
}
void UPauseMenuWidget::OnLoadButtonClicked() {
  SaveSlotsWidget->SetVisibility(ESlateVisibility::Visible);

  SaveSlotsWidget->SaveManagerRef = SaveManagerRef;
  SaveSlotsWidget->bIsSaving = false;
  SaveSlotsWidget->CloseSaveSlotsUI = [this]() { SaveSlotsWidget->SetVisibility(ESlateVisibility::Collapsed); };

  SaveSlotsWidget->RefreshUI();
}

void UPauseMenuWidget::RefreshUI() {
  if (SaveManagerRef->CanSave()) SaveButton->SetIsEnabled(true);
  else SaveButton->SetIsEnabled(false);
}