#include "store_playground/Inventory/InventoryComponent.h"

FInventoryTransferRes TransferItem(UInventoryComponent* From,
                                   UInventoryComponent* To,
                                   UItemBase* Item,
                                   int32 Quantity) {
  if (!From->ItemsArray.ContainsByPredicate([Item](UItemBase* ArrayItem) { return ArrayItem->ItemID == Item->ItemID; }))
    return FInventoryTransferRes{false};

  switch (To->InventoryType) {
    case EInventoryType::Container:
    case EInventoryType::Store: {
      if (!To->ItemsArray.ContainsByPredicate(
              [Item](UItemBase* ArrayItem) { return ArrayItem->ItemID == Item->ItemID; }))
        if (To->ItemsArray.Num() >= To->MaxSlots) return FInventoryTransferRes{false};
      break;
    }
    case EInventoryType::StockDisplay: {
      if (To->ItemsArray.ContainsByPredicate(
              [Item](UItemBase* ArrayItem) { return ArrayItem->ItemID == Item->ItemID; }))
        return FInventoryTransferRes{false};
      if (To->ItemsArray.Num() >= To->MaxSlots) return FInventoryTransferRes{false};
      break;
    }
  }

  switch (From->InventoryType) {
    case EInventoryType::Container:
    case EInventoryType::StockDisplay:
      From->RemoveItem(Item, Quantity);
      To->AddItem(Item, Quantity);
      break;
    case EInventoryType::Store: To->AddItem(Item, Quantity); break;
  }

  return FInventoryTransferRes{true};
}
