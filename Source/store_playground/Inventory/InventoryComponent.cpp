
#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent() {}

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
      AddItem(Item, 1);
    }
  }
}

void UInventoryComponent::AddItem(UItemBase* Item, int32 Amount) {
  if (ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->UniqueItemID == Item->UniqueItemID; })) {
    Item->Quantity += Amount;
  } else {
    Item->Quantity = Amount;
    ItemsArray.Add(Item->CreateItemCopy());
  }

  UIOnInventoryUpdated.Broadcast();
}

void UInventoryComponent::RemoveItem(UItemBase* Item, int32 Amount) {
  if (ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->UniqueItemID == Item->UniqueItemID; })) {
    Item->Quantity -= Amount;
    if (Item->Quantity <= 0) ItemsArray.Remove(Item);
  }

  UIOnInventoryUpdated.Broadcast();
}