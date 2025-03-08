
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
    if (FItemData* ItemData = ItemRowHandle.GetRow<FItemData>(ItemRowHandle.RowName.ToString())) {
      UItemBase* Item = NewObject<UItemBase>(this);

      Item->ItemID = ItemData->ItemID;
      // Item->Quantity = std::clamp(Quantity, 1, ItemData->MetaData.MaxStackSize);
      Item->Quantity = 1;
      Item->UniqueItemID = FGuid::NewGuid();
      Item->FlavorData = ItemData->FlavorData;
      Item->MetaData = ItemData->MetaData;
      Item->AssetData = ItemData->AssetData;
      Item->MarketData = ItemData->MarketData;
      AddItem(Item, 1);
    }
  }
}

void UInventoryComponent::AddItem(const UItemBase* Item, int32 Quantity) {
  if (TObjectPtr<UItemBase>* ArrayItem = ItemsArray.FindByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->UniqueItemID == Item->UniqueItemID; })) {
    if (InventoryType == EInventoryType::Container) (*ArrayItem)->Quantity += Quantity;
    return;
  }

  UItemBase* ItemCopy = Item->CreateItemCopy();
  ItemCopy->Quantity = Quantity;
  ItemsArray.Add(ItemCopy);
}

void UInventoryComponent::RemoveItem(const UItemBase* Item, int32 Quantity) {
  if (TObjectPtr<UItemBase>* ArrayItem = ItemsArray.FindByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->UniqueItemID == Item->UniqueItemID; })) {
    (*ArrayItem)->Quantity -= Quantity;
    if ((*ArrayItem)->Quantity <= 0) ItemsArray.RemoveSingle(*ArrayItem);
  }
}