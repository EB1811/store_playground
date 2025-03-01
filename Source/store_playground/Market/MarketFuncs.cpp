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

  TransferItem(NPCStoreInventory, PlayerInventory, Item, Quantity);
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

  TransferItem(PlayerInventory, NPCStoreInventory, Item, Quantity);
  PlayerStore->Money += Item->MarketData.CurrentPrice * Quantity;
}