#include "Market.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"

void BuyItem(AMarketEconomy* MarketEconomy,
             UInventoryComponent* NPCStoreInventory,
             UInventoryComponent* PlayerInventory,
             AStore* PLayerStore,
             UItemBase* Item,
             int32 Quantity) {
  check(NPCStoreInventory && PlayerInventory && PLayerStore && Item);
  if (!NPCStoreInventory->ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->ItemID == Item->ItemID; }))
    return;

  // TODO: Add prices
  const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
      [Item](const FEconItem& EconItem) { return EconItem.ItemID == Item->ItemID; });
  check(EconItem);
  if (PLayerStore->Money < EconItem->CurrentPrice * Quantity) return;

  if (TransferItem(NPCStoreInventory, PlayerInventory, Item, Quantity).bSuccess)
    PLayerStore->Money -= EconItem->CurrentPrice * Quantity;
}

void SellItem(AMarketEconomy* MarketEconomy,
              UInventoryComponent* NPCStoreInventory,
              UInventoryComponent* PlayerInventory,
              AStore* PlayerStore,
              UItemBase* Item,
              int32 Quantity) {
  check(NPCStoreInventory && PlayerInventory && PlayerStore && Item);
  if (!PlayerInventory->ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->ItemID == Item->ItemID; }))
    return;

  const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
      [Item](const FEconItem& EconItem) { return EconItem.ItemID == Item->ItemID; });
  check(EconItem);
  if (TransferItem(PlayerInventory, NPCStoreInventory, Item, Quantity).bSuccess)
    PlayerStore->Money += EconItem->CurrentPrice * Quantity;
}