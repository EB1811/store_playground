#include "ItemsWidget.h"
#include "Logging/LogVerbosity.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/UI/Inventory/ItemSlotWidget.h"
#include "store_playground/UI/Inventory/ItemDetailsWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Kismet/GameplayStatics.h"

void UItemsWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  check(SelectSound);

  SortData = {.SortType = ESortType::None, .bReverse = false};
  FilterData = {.ItemType = EItemType::Weapon, .bFilterByType = false};
}

void UItemsWidget::SelectItem(const UItemBase* Item, UItemSlotWidget* ItemSlotWidget) {
  check(Item && ItemSlotWidget);

  if (SelectedItem == Item) return;

  SelectedItem = Item;
  ItemDetailsWidget->InitUI(SelectedItem, ShowPriceText, MarketPriceFunc, ShowPriceFunc);
  ItemDetailsWidget->RefreshUI();

  if (SelectedItemSlotWidget) {
    SelectedItemSlotWidget->bIsSelected = false;
    SelectedItemSlotWidget->RefreshUI();
  }
  SelectedItemSlotWidget = ItemSlotWidget;
  ItemSlotWidget->bIsSelected = true;
  ItemSlotWidget->RefreshUI();

  UGameplayStatics::PlaySound2D(this, SelectSound, 1.0f);
}
void UItemsWidget::SelectNextItem(FVector2D Direction) {
  if (SortedItems.IsEmpty()) return;

  if (!SelectedItem) {
    UItemSlotWidget* ItemSlotWidget = Cast<UItemSlotWidget>(ItemsPanelWrapBox->GetChildAt(0));
    SelectItem(SortedItems[0], ItemSlotWidget);
    return;
  }

  UE_LOG(LogTemp, Verbose, TEXT("1: %s"), *SelectedItemSlotWidget->GetDesiredSize().ToString());
  UE_LOG(LogTemp, Verbose, TEXT("2: %s"), *ItemsPanelWrapBox->GetDesiredSize().ToString());
  UE_LOG(LogTemp, Verbose, TEXT("3: %s"), *Direction.ToString());
  // Note: Don't know how robust this is, but seems solid enough.
  auto WrapBoxSize = ItemsPanelWrapBox->GetDesiredSize();
  auto ItemSlotSize = SelectedItemSlotWidget->GetDesiredSize();
  int32 ItemsPerRow = FMath::FloorToInt(WrapBoxSize.X / ItemSlotSize.X);

  int32 NextIndexDirection = 0;
  if (Direction.Y > 0) NextIndexDirection = 1;                  // Right
  else if (Direction.Y < 0) NextIndexDirection = -1;            // Left
  else if (Direction.X > 0) NextIndexDirection = ItemsPerRow;   // Down
  else if (Direction.X < 0) NextIndexDirection = -ItemsPerRow;  // Up

  int32 CurrentIndex = SortedItems.IndexOfByKey(SelectedItem);
  int32 NextIndex = CurrentIndex + NextIndexDirection;
  if (NextIndex < 0 || NextIndex >= SortedItems.Num()) return;  // No wraping.

  UItemSlotWidget* ItemSlotWidget = Cast<UItemSlotWidget>(ItemsPanelWrapBox->GetChildAt(NextIndex));
  SelectItem(SortedItems[NextIndex], ItemSlotWidget);
}

void UItemsWidget::SortItems(FSortData _SortData) {
  SortData = _SortData;
  RefreshUI();
}
void UItemsWidget::FilterItems(FItemsFilterData _FilterData) {
  FilterData = _FilterData;
  RefreshUI();
}

void UItemsWidget::RefreshUI() {
  check(InventoryRef);

  ItemsPanelWrapBox->ClearChildren();
  ItemDetailsWidget->SetVisibility(ESlateVisibility::Hidden);

  SortedItems.Empty();
  SortedItems.Reserve(InventoryRef->ItemsArray.Num());
  for (UItemBase* Item : InventoryRef->ItemsArray) SortedItems.Add(Item);

  if (FilterData.bFilterByType)
    SortedItems.RemoveAllSwap([this](const UItemBase* Item) { return Item->ItemType != FilterData.ItemType; });
  if (SortedItems.IsEmpty()) {
    SelectedItem = nullptr;
    ItemDetailsWidget->SetVisibility(ESlateVisibility::Collapsed);
    return;
  }

  if (SortData.SortType != ESortType::None) {
    Algo::Sort(SortedItems, [this](const UItemBase* A, const UItemBase* B) {
      switch (SortData.SortType) {
        case ESortType::Price:
          return ShowPriceFunc ? ShowPriceFunc(A->ItemID) > ShowPriceFunc(B->ItemID)
                               : MarketPriceFunc(A->ItemID) > MarketPriceFunc(B->ItemID);
        case ESortType::Name: return A->TextData.Name.ToString() < B->TextData.Name.ToString();
        default: return false;
      }
    });

    if (SortData.bReverse) Algo::Reverse(SortedItems);
  }

  if (!SelectedItem) SelectedItem = SortedItems[0];
  else if (!SortedItems.Contains(SelectedItem)) SelectedItem = SortedItems[0];

  ItemDetailsWidget->InitUI(SelectedItem, ShowPriceText, MarketPriceFunc, ShowPriceFunc);
  ItemDetailsWidget->RefreshUI();
  ItemDetailsWidget->SetVisibility(ESlateVisibility::Visible);

  for (const UItemBase* Item : SortedItems) {
    UItemSlotWidget* ItemSlotWidget = CreateWidget<UItemSlotWidget>(this, ItemSlotWidgetClass);
    check(ItemSlotWidget);

    if (Item == SelectedItem) {
      ItemSlotWidget->bIsSelected = true;
      SelectedItemSlotWidget = ItemSlotWidget;
    }
    ItemSlotWidget->ItemRef = Item;
    ItemSlotWidget->ShowPriceFunc = ShowPriceFunc ? ShowPriceFunc : MarketPriceFunc;
    ItemSlotWidget->OnSelectItemFunc = [this, ItemSlotWidget](const UItemBase* Item) {
      SelectItem(Item, ItemSlotWidget);
    };
    ItemSlotWidget->RefreshUI();

    ItemsPanelWrapBox->AddChildToWrapBox(ItemSlotWidget);
  }
}

void UItemsWidget::InitUI(const class UInventoryComponent* _InventoryRef,
                          FName _ShowPriceText,
                          std::function<float(FName)> _MarketPriceFunc,
                          std::function<float(FName)> _ShowPriceFunc) {
  check(_InventoryRef && _MarketPriceFunc && ItemSlotWidgetClass);

  InventoryRef = _InventoryRef;
  ShowPriceText = _ShowPriceText;
  MarketPriceFunc = _MarketPriceFunc;
  ShowPriceFunc = _ShowPriceFunc;

  FilterData = {.ItemType = EItemType::Weapon, .bFilterByType = false};
}