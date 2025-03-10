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
  ItemCopy->PriceData = PriceData;

  return ItemCopy;
}