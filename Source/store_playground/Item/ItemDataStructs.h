// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStructs.generated.h"

UENUM()
enum class EItemType : uint8 {
  Weapon UMETA(DisplayName = "Weapon"),
  Armor UMETA(DisplayName = "Armor"),
  Jewellery UMETA(DisplayName = "Jewellery"),
  Clothing UMETA(DisplayName = "Clothing"),
  Consumable UMETA(DisplayName = "Consumable"),
  Valuables UMETA(DisplayName = "Valuables"),
  Objects UMETA(DisplayName = "Objects"),
  Financial UMETA(DisplayName = "Financial"),
};
UENUM()
enum class EItemWealthType : uint8 {
  Essential UMETA(DisplayName = "Essential"),
  Mid UMETA(DisplayName = "Mid"),
  Luxury UMETA(DisplayName = "Luxury"),
};
ENUM_RANGE_BY_COUNT(EItemWealthType, 3);

UENUM()
enum class EItemEconType : uint8 {
  Consumer UMETA(DisplayName = "Consumer"),
  Adventurer UMETA(DisplayName = "Adventurer"),
  Magic UMETA(DisplayName = "Magic"),
  Finance UMETA(DisplayName = "Finance"),
};
ENUM_RANGE_BY_COUNT(EItemEconType, 4);

USTRUCT()
struct FItemEconData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FGuid ItemID;

  UPROPERTY(EditAnywhere)
  EItemType Type;
  UPROPERTY(EditAnywhere)
  EItemWealthType WealthType;
  UPROPERTY(EditAnywhere)
  EItemEconType EconType;
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
};

USTRUCT()
struct FItemMetaData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  bool bStackable;
  UPROPERTY(EditAnywhere)
  bool bUnique;
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

// Outdated, use FItemDataRow instead.
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

USTRUCT()
struct FItemDataRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ItemID;

  UPROPERTY(EditAnywhere)
  FItemTextData TextData;

  UPROPERTY(EditAnywhere)
  EItemType ItemType;
  UPROPERTY(EditAnywhere)
  EItemWealthType ItemWealthType;
  UPROPERTY(EditAnywhere)
  EItemEconType ItemEconType;
  UPROPERTY(EditAnywhere)
  float BasePrice;
  UPROPERTY(EditAnywhere)
  float PriceJumpPercent;  // * Reverse price stickiness, 1.0 = current price changes to perfect price in one cycle.

  UPROPERTY(EditAnywhere)
  FItemAssetData AssetData;
  UPROPERTY(EditAnywhere)
  FItemFlavorData FlavorData;

  UPROPERTY(EditAnywhere)
  FItemMetaData MetaData;
};