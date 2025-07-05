
#include "InventoryComponent.h"
#include "store_playground/Item/ItemBase.h"

UInventoryComponent::UInventoryComponent() {
  PrimaryComponentTick.bCanEverTick = false;
  InventoryType = EInventoryType::Container;
  MaxSlots = 30;
}

void UInventoryComponent::BeginPlay() {
  Super::BeginPlay();

  for (FDataTableRowHandle ItemRowHandle : InitItemIds) {
    if (FItemDataRow* Row = ItemRowHandle.GetRow<FItemDataRow>(ItemRowHandle.RowName.ToString())) {
      UItemBase* Item = NewObject<UItemBase>(this);

      Item->ItemID = Row->ItemID;
      Item->UniqueItemID = FGuid::NewGuid();
      Item->Quantity = 1;
      Item->ItemType = Row->ItemType;
      Item->ItemWealthType = Row->ItemWealthType;
      Item->ItemEconType = Row->ItemEconType;
      Item->TextData = Row->TextData;
      Item->AssetData = Row->AssetData;
      Item->FlavorData = Row->FlavorData;
      Item->PlayerPriceData.BoughtAt = 0.0f;
      AddItem(Item, 1);
    }
  }
}

auto UInventoryComponent::AddItem(const UItemBase* Item, int32 Quantity) -> UItemBase* {
  if (TObjectPtr<UItemBase>* ExistingItem = ItemsArray.FindByPredicate(
          [this, Item](UItemBase* ArrayItem) { return AreItemsEqual(InventoryType, ArrayItem, Item); })) {
    if (InventoryType == EInventoryType::Container || InventoryType == EInventoryType::PlayerInventory)
      (*ExistingItem)->Quantity += Quantity;

    return (*ExistingItem);
  }

  UItemBase* ItemCopy = Item->CreateItemCopy();
  ItemCopy->Quantity = Quantity;
  ItemsArray.Add(ItemCopy);

  return ItemCopy;
}

void UInventoryComponent::RemoveItem(const UItemBase* Item, int32 Quantity) {
  if (TObjectPtr<UItemBase>* ArrayItem = ItemsArray.FindByPredicate(
          [this, Item](UItemBase* ArrayItem) { return AreItemsEqual(InventoryType, ArrayItem, Item); })) {
    (*ArrayItem)->Quantity -= Quantity;
    if ((*ArrayItem)->Quantity <= 0) ItemsArray.RemoveSingle(*ArrayItem);
  }
}