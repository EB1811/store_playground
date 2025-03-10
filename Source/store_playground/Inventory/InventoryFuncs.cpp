#include "store_playground/Inventory/InventoryComponent.h"

FInventoryTransferRes TransferItem(UInventoryComponent* From,
                                   UInventoryComponent* To,
                                   UItemBase* Item,
                                   int32 Quantity) {
  if (!From->ItemsArray.ContainsByPredicate([Item](UItemBase* ArrayItem) { return ArrayItem->ItemID == Item->ItemID; }))
    return FInventoryTransferRes{false};

  if (!To->ItemsArray.ContainsByPredicate([Item](UItemBase* ArrayItem) { return ArrayItem->ItemID == Item->ItemID; }))
    if (To->ItemsArray.Num() >= To->MaxSlots) return FInventoryTransferRes{false};

  switch (From->InventoryType) {
    case EInventoryType::Container:
      From->RemoveItem(Item, Quantity);
      To->AddItem(Item, Quantity);
      break;
    case EInventoryType::Store: To->AddItem(Item, Quantity); break;
  }

  return FInventoryTransferRes{true};
}
