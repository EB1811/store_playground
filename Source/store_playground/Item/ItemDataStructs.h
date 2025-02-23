// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStructs.generated.h"

UENUM()
enum class EItemType : uint8 {
  Weapon UMETA(DisplayName = "Weapon"),
  Armor UMETA(DisplayName = "Armor"),
  Consumable UMETA(DisplayName = "Consumable"),
  Spell UMETA(DisplayName = "Spell"),
  Relic UMETA(DisplayName = "Relic"),
};

UENUM()
enum class EItemQuality : uint8 {
  Improvised UMETA(DisplayName = "Improvised"),
  Common UMETA(DisplayName = "Common"),
  Quality UMETA(DisplayName = "Quality"),
  Masterwork UMETA(DisplayName = "Masterwork"),
  Treasure UMETA(DisplayName = "Treasure"),
};

USTRUCT()
struct FItemStats {
  GENERATED_BODY()

  //
  UPROPERTY(EditAnywhere)
  int32 Attack;
  UPROPERTY(EditAnywhere)
  int32 Defense;
  UPROPERTY(EditAnywhere)
  int32 Durability;
  UPROPERTY(EditAnywhere)
  float Weight;
};

USTRUCT() struct FItemTextData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FText Name;
  UPROPERTY(EditAnywhere)
  FText Description;
};

USTRUCT()
struct FItemFlavorData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  EItemType Type;
  UPROPERTY(EditAnywhere)
  EItemQuality Quality;
  UPROPERTY(EditAnywhere)
  FItemStats StatsData;
  UPROPERTY(EditAnywhere)
  FItemTextData TextData;
};

USTRUCT()
struct FItemMetaData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  int32 MaxStackSize;
  UPROPERTY(EditAnywhere)
  bool Unique;
  UPROPERTY(EditAnywhere)
  bool Stackable;
};

USTRUCT()
struct FItemAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  class UTexture2D* Icon;
  UPROPERTY(EditAnywhere)
  class UStaticMesh* Mesh;
};

USTRUCT()
struct FItemMarketData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float BasePrice;
  // * CurrentPrice comes from the BasePrice but also is modified by external.
  UPROPERTY(EditAnywhere)
  float CurrentPrice;
  UPROPERTY(EditAnywhere)
  int32 SupplyUnits;
  UPROPERTY(EditAnywhere)
  int32 DemandUnits;
};

USTRUCT()
struct FItemData : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, Category = "Item Data")
  FName ItemID;

  // UPROPERTY(EditAnywhere, Category = "Item Data")
  // int32 Quantity;

  UPROPERTY(EditAnywhere, Category = "Item Data")
  FItemFlavorData FlavorData;
  UPROPERTY(EditAnywhere, Category = "Item Data")
  FItemMetaData MetaData;
  UPROPERTY(EditAnywhere, Category = "Item Data")
  FItemAssetData AssetData;
  UPROPERTY(EditAnywhere, Category = "Item Data")
  FItemMarketData MarketData;
};