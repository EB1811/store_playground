#pragma once

#include "ItemBase.h"
#include "CoreMinimal.h"
#include "ItemDataStructs.h"

UItemBase::UItemBase() {}

UItemBase* UItemBase::CreateItemCopy() const {
  UItemBase* ItemCopy = NewObject<UItemBase>(UItemBase::StaticClass());
  ItemCopy->ItemID = ItemID;
  ItemCopy->UniqueItemID = UniqueItemID;
  ItemCopy->Quantity = Quantity;
  ItemCopy->FlavorData = FlavorData;
  ItemCopy->MetaData = MetaData;
  ItemCopy->AssetData = AssetData;
  ItemCopy->MarketData = MarketData;

  return ItemCopy;
}