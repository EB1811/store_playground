// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Types/SlateEnums.h"
#include "ItemDataStructs.generated.h"

UENUM()
enum class EItemType : uint8 {
  Weapon UMETA(DisplayName = "Weapon"),
  Armor UMETA(DisplayName = "Armor"),
  Jewellery UMETA(DisplayName = "Jewellery"),
  Clothing UMETA(DisplayName = "Clothing"),
  Consumable UMETA(DisplayName = "Consumable"),
  Valuables UMETA(DisplayName = "Valuable"),
  Objects UMETA(DisplayName = "Object"),
  Financial UMETA(DisplayName = "Financial"),
};
ENUM_RANGE_BY_COUNT(EItemType, 8);

// UENUM()
// enum class EItemMetaType : uint8 {
//   Unique UMETA(DisplayName = "Unique"),
//   NonUnique UMETA(DisplayName = "NonUnique"),
// };
// ENUM_RANGE_BY_COUNT(EItemMetaType, 2);

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
struct FItemTextData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FText Name;
  UPROPERTY(EditAnywhere, meta = (MultiLine = true))
  FText Description;
};

USTRUCT()
struct FItemAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  class UTexture2D* Icon;
  UPROPERTY(EditAnywhere)
  class UPaperFlipbook* Sprite;
};

UENUM()
enum class EItemFlavorType : uint8 {
  None UMETA(DisplayName = "None"),
  Quality UMETA(DisplayName = "Quality"),  // Each new instance of an item, has a random quality.
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
struct FItemFlavorData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  EItemFlavorType FlavorType;
};

USTRUCT()
struct FItemDataRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ItemID;

  UPROPERTY(EditAnywhere)
  EItemType ItemType;
  UPROPERTY(EditAnywhere)
  bool bIsUnlockable;

  UPROPERTY(EditAnywhere)
  FItemTextData TextData;
  UPROPERTY(EditAnywhere)
  FItemAssetData AssetData;

  UPROPERTY(EditAnywhere)
  EItemWealthType ItemWealthType;
  UPROPERTY(EditAnywhere)
  EItemEconType ItemEconType;
  UPROPERTY(EditAnywhere)
  float BasePrice;
  UPROPERTY(EditAnywhere)
  float PriceJumpPercent;  // * Reverse price stickiness, 1.0 = current price changes to perfect price in one cycle.

  UPROPERTY(EditAnywhere)
  FItemFlavorData FlavorData;
};