// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDataStructs.h"
#include "ItemBase.generated.h"

UCLASS() class STORE_PLAYGROUND_API UItemBase : public UObject
{
  GENERATED_BODY()

public:
  UItemBase();

  UPROPERTY(EditAnywhere, Category = "Item Data")
  FName ItemID;
  // * Each item will have dynamic flavor data, resulting in different unique item IDs from the same base item ID.
  // ? Is this the best way to handle this?
  // ? Is ItemID needed?
  UPROPERTY(EditAnywhere, Category = "Item Data")
  FGuid UniqueItemID;

  UPROPERTY(EditAnywhere, Category = "Item Data", meta = (UIMin = 1))
  int16 Quantity;

  UPROPERTY(EditAnywhere, Category = "Item Data | Flavor")
  FItemFlavorData FlavorData;
  UPROPERTY(EditAnywhere, Category = "Item Data")
  FItemMetaData MetaData;
  UPROPERTY(EditAnywhere, Category = "Item Data")
  FItemAssetData AssetData;

  UPROPERTY(EditAnywhere, Category = "Item Data | Market")
  FItemMarketData MarketData;

  // UPROPERTY()
  // class UInventoryComponent* OwningInventory;

  UFUNCTION(Category = "Item Functions")
  UItemBase* CreateItemCopy() const;
};