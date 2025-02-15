#include "store_playground/Inventory/InventoryComponent.h"

void TransferItem(UInventoryComponent* From, UInventoryComponent* To, UItemBase* Item, int32 Quantity) {
  if (!From->ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->UniqueItemID == Item->UniqueItemID; }))
    return;

  switch (From->InventoryType) {
    case EInventoryType::Container:
      From->RemoveItem(Item, Quantity);
      To->AddItem(Item, Quantity);
      break;
    case EInventoryType::Store: To->AddItem(Item, Quantity); break;
  }
}
