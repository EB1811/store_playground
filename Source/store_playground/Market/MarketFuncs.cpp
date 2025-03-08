#include "Market.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Store/Store.h"

void BuyItem(UInventoryComponent* NPCStoreInventory,
             UInventoryComponent* PlayerInventory,
             AStore* PLayerStore,
             UItemBase* Item,
             int32 Quantity) {
  check(NPCStoreInventory && PlayerInventory && PLayerStore && Item);
  if (!NPCStoreInventory->ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->UniqueItemID == Item->UniqueItemID; }))
    return;

  if (PLayerStore->Money < Item->MarketData.CurrentPrice * Quantity) return;

  if (TransferItem(NPCStoreInventory, PlayerInventory, Item, Quantity).bSuccess)
    PLayerStore->Money -= Item->MarketData.CurrentPrice * Quantity;
}

void SellItem(UInventoryComponent* NPCStoreInventory,
              UInventoryComponent* PlayerInventory,
              AStore* PlayerStore,
              UItemBase* Item,
              int32 Quantity) {
  check(NPCStoreInventory && PlayerInventory && PlayerStore && Item);
  if (!PlayerInventory->ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->UniqueItemID == Item->UniqueItemID; }))
    return;

  if (TransferItem(PlayerInventory, NPCStoreInventory, Item, Quantity).bSuccess)
    PlayerStore->Money += Item->MarketData.CurrentPrice * Quantity;
}