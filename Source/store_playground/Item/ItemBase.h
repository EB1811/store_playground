// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDataStructs.h"
#include "ItemBase.generated.h"

USTRUCT()
struct FItemPriceData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float BoughtAt;
};

UCLASS() class STORE_PLAYGROUND_API UItemBase : public UObject {
  GENERATED_BODY()

public:
  UItemBase();

  UPROPERTY(EditAnywhere, Category = "Item Data")
  FName ItemID;
  UPROPERTY(EditAnywhere, Category = "Item Data")
  FGuid UniqueItemID;

  UPROPERTY(EditAnywhere, Category = "Item Data", meta = (UIMin = 1))
  int32 Quantity;

  UPROPERTY(EditAnywhere)
  EItemType ItemType;

  UPROPERTY(EditAnywhere)
  EItemWealthType ItemWealthType;
  UPROPERTY(EditAnywhere)
  EItemEconType ItemEconType;

  UPROPERTY(EditAnywhere, Category = "Item Data")
  FItemTextData TextData;
  UPROPERTY(EditAnywhere, Category = "Item Data")
  FItemAssetData AssetData;

  UPROPERTY(EditAnywhere, Category = "Item Data")
  FItemFlavorData FlavorData;

  UPROPERTY(EditAnywhere, Category = "Item Data")
  FItemPriceData PriceData;

  // UPROPERTY()
  // class UInventoryComponent* OwningInventory;

  UFUNCTION(Category = "Item Functions")
  UItemBase* CreateItemCopy() const;
};