#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Item/ItemBase.h"

auto AreItemsEqual(EInventoryType InventoryType, const UItemBase* Item1, const UItemBase* Item2) -> bool {
  check(Item1 && Item2);

  if (Item1->ItemID != Item2->ItemID) return false;
  if (!FMath::IsNearlyEqual(Item1->PlayerPriceData.BoughtAt, Item2->PlayerPriceData.BoughtAt, KINDA_SMALL_NUMBER) &&
      InventoryType != EInventoryType::Store)
    return false;

  return true;
}

auto CanTransferItem(const UInventoryComponent* To, const UItemBase* Item) -> bool {
  if (To->InventoryType == EInventoryType::StockDisplay &&
      To->ItemsArray.ContainsByPredicate(
          [To, Item](UItemBase* ArrayItem) { return AreItemsEqual(To->InventoryType, ArrayItem, Item); }))
    return false;

  if (!To->ItemsArray.ContainsByPredicate(
          [To, Item](UItemBase* ArrayItem) { return AreItemsEqual(To->InventoryType, ArrayItem, Item); }) &&
      To->ItemsArray.Num() >= To->MaxSlots)
    return false;

  return true;
}

auto TransferItem(UInventoryComponent* From,
                  UInventoryComponent* To,
                  UItemBase* Item,
                  int32 Quantity) -> FInventoryTransferRes {
  check(From->ItemsArray.ContainsByPredicate(
      [Item](UItemBase* ArrayItem) { return ArrayItem->UniqueItemID == Item->UniqueItemID; }));

  if (!CanTransferItem(To, Item)) return FInventoryTransferRes{false};

  switch (From->InventoryType) {
    case EInventoryType::Container:
    case EInventoryType::StockDisplay:
    case EInventoryType::PlayerInventory: From->RemoveItem(Item, Quantity); break;

    case EInventoryType::Store: break;
  }

  UItemBase* ItemCopy = To->AddItem(Item, Quantity);
  return FInventoryTransferRes{true, ItemCopy};
}