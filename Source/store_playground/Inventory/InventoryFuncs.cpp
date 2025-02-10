#include "store_playground/Inventory/InventoryComponent.h"

void TransferItem(UInventoryComponent* From, UInventoryComponent* To, UItemBase* Item, int32 Amount = 1) {
  if (From->ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->UniqueItemID == Item->UniqueItemID; })) {
    From->RemoveItem(Item, Amount);
    To->AddItem(Item, Amount);
  }
}