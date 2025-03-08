#include "InventoryWidget.h"
#include "store_playground/UI/Inventory/InventoryItemSlotWidget.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/UI/Inventory/ItemDragDropOp.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UInventoryWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

bool UInventoryWidget::NativeOnDrop(const FGeometry& InGeometry,
                                    const FDragDropEvent& InDragDropEvent,
                                    UDragDropOperation* InOperation) {
  check(InventoryRef);

  const UItemDragDropOp* ItemDragDropOp = Cast<UItemDragDropOp>(InOperation);
  if (!ItemDragDropOp) return false;

  UItemBase* DroppedItem = ItemDragDropOp->SourceItem;
  UInventoryComponent* SourceInventoryRef = ItemDragDropOp->SourceInventory;
  UInventoryWidget* SourceInventoryWidgetRef = ItemDragDropOp->SourceInventoryWidget;
  check(DroppedItem && SourceInventoryRef && SourceInventoryWidgetRef);

  if (SourceInventoryRef == InventoryRef) return false;

  // TODO: Add success/failure return struct.
  if (OnDropItemFunc)
    // TODO: Add source inventory parameter.
    OnDropItemFunc(DroppedItem, 1);
  else
    TransferItem(SourceInventoryRef, InventoryRef, DroppedItem);

  RefreshInventory();
  SourceInventoryWidgetRef->RefreshInventory();

  return true;
}

void UInventoryWidget::RefreshInventory() {
  check(InventoryRef && ItemSlotClass);

  InventoryPanelWrapBox->ClearChildren();

  for (UItemBase* Item : InventoryRef->ItemsArray) {
    UE_LOG(LogTemp, Warning, TEXT("RefreshInventory, Quantity: %d"), Item->Quantity);
    UInventoryItemSlotWidget* InventoryItemSlot = CreateWidget<UInventoryItemSlotWidget>(this, ItemSlotClass);
    InventoryItemSlot->ItemRef = Item;
    InventoryItemSlot->InventoryWidgetRef = this;
    // TODO: Select mode + single select mode.
    InventoryItemSlot->OnSelectItemFunc = [this, InventoryItemSlot](UItemBase* Item) {
      if (SelectedItemSlotWidget)
        SelectedItemSlotWidget->SelectItemButton->SetBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));

      SelectedItem = Item;
      SelectedItemSlotWidget = InventoryItemSlot;
      SelectedItemSlotWidget->SelectItemButton->SetBackgroundColor(FLinearColor(0.0f, 1.0f, 0.0f, 0.5f));
    };

    InventoryPanelWrapBox->AddChildToWrapBox(InventoryItemSlot);
  }
}