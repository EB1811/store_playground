#pragma once

#include "ItemBase.h"
#include "CoreMinimal.h"
#include "ItemDataStructs.h"

UItemBase::UItemBase() {}

UItemBase* UItemBase::CreateItemCopy() const {
  UItemBase* ItemCopy = NewObject<UItemBase>(UItemBase::StaticClass());
  ItemCopy->ItemID = ItemID;
  ItemCopy->UniqueItemID = FGuid::NewGuid();
  ItemCopy->Quantity = Quantity;
  ItemCopy->ItemType = ItemType;
  ItemCopy->ItemWealthType = ItemWealthType;
  ItemCopy->ItemEconType = ItemEconType;
  ItemCopy->TextData = TextData;
  ItemCopy->AssetData = AssetData;
  ItemCopy->FlavorData = FlavorData;
  ItemCopy->PlayerPriceData = PlayerPriceData;

  return ItemCopy;
}

void UItemBase::SetItemFromBase(const UItemBase* ItemBase) {
  ItemType = ItemBase->ItemType;
  ItemWealthType = ItemBase->ItemWealthType;
  ItemEconType = ItemBase->ItemEconType;
  TextData = ItemBase->TextData;
  AssetData = ItemBase->AssetData;
  FlavorData = ItemBase->FlavorData;
}