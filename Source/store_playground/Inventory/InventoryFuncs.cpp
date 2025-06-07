#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Item/ItemBase.h"

auto TransferItem(UInventoryComponent* From,
                  UInventoryComponent* To,
                  UItemBase* Item,
                  int32 Quantity) -> FInventoryTransferRes {
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

  UItemBase* ItemCopy = nullptr;
  switch (From->InventoryType) {
    case EInventoryType::Container:
    case EInventoryType::StockDisplay:
      From->RemoveItem(Item, Quantity);
      ItemCopy = To->AddItem(Item, Quantity);
      break;
    case EInventoryType::Store: ItemCopy = To->AddItem(Item, Quantity); break;
  }

  return FInventoryTransferRes{true, ItemCopy};
}