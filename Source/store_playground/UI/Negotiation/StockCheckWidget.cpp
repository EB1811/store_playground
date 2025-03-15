#include "StockCheckWidget.h"
#include "store_playground/UI/Inventory/InventoryItemSlotWidget.h"
#include "store_playground/UI/Inventory/InventoryWidget.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/UI/Inventory/ItemDragDropOp.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UStockCheckWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ShowItemButton->OnClicked.AddDynamic(this, &UStockCheckWidget::OnShowItemButtonClicked);
}

void UStockCheckWidget::InitStockCheckUI(UInventoryComponent* PlayerInventory, const FText ItemTypeName) {
  PlayerInventoryWidget->InventoryRef = PlayerInventory;
  PlayerInventoryWidget->RefreshInventory();

  WantedItemTypeName->SetText(ItemTypeName);
}

void UStockCheckWidget::OnShowItemButtonClicked() {
  check(ShowItemFunc);
  if (PlayerInventoryWidget->SelectedItem)
    ShowItemFunc(PlayerInventoryWidget->SelectedItem, PlayerInventoryWidget->InventoryRef);
}