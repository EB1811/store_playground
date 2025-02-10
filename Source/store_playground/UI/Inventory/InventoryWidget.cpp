#include "InventoryWidget.h"
#include "store_playground/UI/Inventory/InventoryItemSlotWidget.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "Components/WrapBox.h"

void UInventoryWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

void UInventoryWidget::RefreshInventory() {
  if (!InventoryRef || !ItemSlotClass) return;

  InventoryPanelWrapBox->ClearChildren();

  for (UItemBase* Item : InventoryRef->ItemsArray) {
    UE_LOG(LogTemp, Warning, TEXT("RefreshInventory"));
    UInventoryItemSlotWidget* InventoryItemSlot = CreateWidget<UInventoryItemSlotWidget>(this, ItemSlotClass);
    InventoryItemSlot->ItemRef = Item;

    InventoryPanelWrapBox->AddChildToWrapBox(InventoryItemSlot);
  }
}