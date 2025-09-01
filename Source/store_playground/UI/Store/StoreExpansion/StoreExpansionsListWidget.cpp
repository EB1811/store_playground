#include "StoreExpansionsListWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "store_playground/UI/Store/StoreExpansion/StoreExpansionSelectWidget.h"
#include "store_playground/UI/Store/StoreExpansion/DisabledStoreExpansionSelectWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"
#include "store_playground/Store/Store.h"
#include "Kismet/GameplayStatics.h"

void UStoreExpansionsListWidget::SelectExpansion(FName ExpansionID, UStoreExpansionSelectWidget* ExpansionCardWidget) {
  check(ExpansionCardWidget);
  if (SelectedExpansionID == ExpansionID) return;

  SelectedExpansionID = ExpansionID;

  if (SelectedExpansionCardWidget) {
    SelectedExpansionCardWidget->bIsSelected = false;
    SelectedExpansionCardWidget->RefreshUI();
  }
  SelectedExpansionCardWidget = ExpansionCardWidget;
  SelectedExpansionCardWidget->bIsSelected = true;
  SelectedExpansionCardWidget->RefreshUI();
  SelectedExpansionCardWidget->SelectButton->SetFocus();

  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);
}
void UStoreExpansionsListWidget::SelectNextExpansion(FVector2D Direction) {
  if (Direction.X == 0) return;
  if (ExpansionsListBox->GetChildrenCount() <= 0) return;

  if (!SelectedExpansionCardWidget) {
    UStoreExpansionSelectWidget* ExpansionCardWidget =
        Cast<UStoreExpansionSelectWidget>(ExpansionsListBox->GetChildAt(0));
    check(ExpansionCardWidget);
    SelectExpansion(ExpansionCardWidget->ExpansionID, ExpansionCardWidget);
    return;
  }

  int32 CurrentIndex = ExpansionsListBox->GetAllChildren().IndexOfByKey(SelectedExpansionCardWidget);
  check(CurrentIndex != INDEX_NONE);
  int32 NextIndex = CurrentIndex + (Direction.X > 0 ? 1 : -1);
  if (NextIndex < 0 || NextIndex >= ExpansionsListBox->GetChildrenCount()) return;

  UStoreExpansionSelectWidget* NextExpansionCardWidget =
      Cast<UStoreExpansionSelectWidget>(ExpansionsListBox->GetChildAt(NextIndex));
  SelectExpansion(NextExpansionCardWidget->ExpansionID, NextExpansionCardWidget);
}

void UStoreExpansionsListWidget::UnlockExpansion() {
  if (SelectedExpansionID.IsNone()) return;

  FStoreExpansionData* ExpansionData = StoreExpansionManager->StoreExpansions.FindByPredicate(
      [&](const FStoreExpansionData& Data) { return Data.StoreExpansionLevelID == SelectedExpansionID; });
  checkf(ExpansionData && Store->Money >= ExpansionData->Price && !ExpansionData->bIsLocked,
         TEXT("Selected expansion is not available or cost exceeds available money, this means "
              "GetAvailableExpansions returned wrong data."));

  StoreExpansionManager->SelectExpansion(SelectedExpansionID);
  Store->MoneySpent(ExpansionData->Price);

  UGameplayStatics::PlaySound2D(this, UnlockSound, 1.0f);
  CloseWidgetFunc();
}

void UStoreExpansionsListWidget::RefreshUI() {
  ExpansionsListBox->ClearChildren();
  LockedExpansionsListBox->ClearChildren();

  SelectedExpansionID = FName();
  SelectedExpansionCardWidget = nullptr;

  for (const FStoreExpansionData& ExpansionData : StoreExpansionManager->StoreExpansions) {
    if (ExpansionData.bIsLocked || Store->Money < ExpansionData.Price) {
      UDisabledStoreExpansionSelectWidget* ExpansionCardWidget =
          CreateWidget<UDisabledStoreExpansionSelectWidget>(this, DisabledStoreExpansionSelectWidgetClass);
      check(ExpansionCardWidget);

      ExpansionCardWidget->NameText->SetText(ExpansionData.Name);
      ExpansionCardWidget->PictureImage->SetBrushFromMaterial(ExpansionData.Picture);
      ExpansionCardWidget->PriceText->SetText(
          FText::FromString(FString::Printf(TEXT("Cost: %.0f¬"), ExpansionData.Price)));
      if (ExpansionData.bIsLocked) ExpansionCardWidget->LockedReasonText->SetText(FText::FromString("Locked"));
      else ExpansionCardWidget->LockedReasonText->SetVisibility(ESlateVisibility::Hidden);

      LockedExpansionsListBox->AddChildToVerticalBox(ExpansionCardWidget);
    } else {
      if (ExpansionData.StoreExpansionLevelID == StoreExpansionManager->CurrentStoreExpansionLevelID) continue;

      UStoreExpansionSelectWidget* ExpansionCardWidget =
          CreateWidget<UStoreExpansionSelectWidget>(this, StoreExpansionSelectWidgetClass);
      check(ExpansionCardWidget);

      ExpansionCardWidget->InitUI(ExpansionData, [this, ExpansionCardWidget](FName ExpansionID) {
        SelectExpansion(ExpansionID, ExpansionCardWidget);
      });
      ExpansionCardWidget->RefreshUI();
      ExpansionsListBox->AddChildToVerticalBox(ExpansionCardWidget);

      if (SelectedExpansionID.IsNone()) {
        SelectedExpansionID = ExpansionData.StoreExpansionLevelID;
        SelectedExpansionCardWidget = ExpansionCardWidget;
        SelectedExpansionCardWidget->bIsSelected = true;
        SelectedExpansionCardWidget->RefreshUI();
      }
    }
  }
}

void UStoreExpansionsListWidget::InitUI(class AStore* _Store,
                                        class AStoreExpansionManager* _StoreExpansionManager,
                                        TFunction<void()> _CloseWidgetFunc) {
  check(_Store && _StoreExpansionManager && _CloseWidgetFunc);

  Store = _Store;
  StoreExpansionManager = _StoreExpansionManager;
  CloseWidgetFunc = _CloseWidgetFunc;

  SelectedExpansionID = FName();
  SelectedExpansionCardWidget = nullptr;
}