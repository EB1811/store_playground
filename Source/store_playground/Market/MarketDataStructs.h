// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "PaperFlipbook.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Sprite/SpriteStructs.h"
#include "MarketDataStructs.generated.h"

UENUM()
enum class EPopType : uint8 {
  Common UMETA(DisplayName = "Common"),
  Adventurer UMETA(DisplayName = "Adventurer"),
  Nobility UMETA(DisplayName = "Nobility"),
  Mystic UMETA(DisplayName = "Mystic"),
  Criminal UMETA(DisplayName = "Criminal"),
  Financial UMETA(DisplayName = "Financial"),
};
ENUM_RANGE_BY_COUNT(EPopType, 6);
UENUM()
enum class EPopWealthType : uint8 {
  Poorer UMETA(DisplayName = "Poorer"),
  Middle UMETA(DisplayName = "Middle"),
  Upper UMETA(DisplayName = "Upper"),
};
ENUM_RANGE_BY_COUNT(EPopWealthType, 3);

USTRUCT()
struct FCustomerPopAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  class UPaperSprite* Sprite;
};
USTRUCT()
struct FCustomerPopDataRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  FText PopName;

  UPROPERTY(EditAnywhere)
  int32 InitPopulation;
  UPROPERTY(EditAnywhere)
  EPopType PopType;
  UPROPERTY(EditAnywhere)
  EPopWealthType WealthType;
  UPROPERTY(EditAnywhere)
  TArray<EItemEconType> ItemEconTypes;

  UPROPERTY(EditAnywhere)
  FCustomerPopAssetData AssetData;
};

// * Alternative to micro managing each pop's economy.
USTRUCT()
struct FBasePopTypeToEconomyRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  EPopType PopType;
  UPROPERTY(EditAnywhere)
  EPopWealthType WealthType;

  UPROPERTY(EditAnywhere)
  int32 MGen;
  UPROPERTY(EditAnywhere)
  float MSharePercent;
  UPROPERTY(EditAnywhere)
  TMap<EItemWealthType, float> ItemSpendPercent;
  UPROPERTY(EditAnywhere)
  TMap<EItemWealthType, int32> ItemNeeds;
};
USTRUCT()
struct FBasePopTypeEconomyData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  int32 MGen;
  UPROPERTY(EditAnywhere)
  float MSharePercent;
  UPROPERTY(EditAnywhere)
  TMap<EItemWealthType, float> ItemSpendPercent;
  UPROPERTY(EditAnywhere)
  TMap<EItemWealthType, int32> ItemNeeds;
};

// * CustomerPop is used for economic details, actual customer data simply uses this.
USTRUCT()
struct FCustomerPop {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;
  UPROPERTY(EditAnywhere)
  FText PopName;

  UPROPERTY(EditAnywhere)
  EPopType PopType;
  UPROPERTY(EditAnywhere)
  EPopWealthType WealthType;

  UPROPERTY(EditAnywhere)
  int32 MGen;
  UPROPERTY(EditAnywhere)
  float MSharePercent;

  UPROPERTY(EditAnywhere)
  TArray<EItemEconType> ItemEconTypes;
  UPROPERTY(EditAnywhere)
  TMap<EItemWealthType, float> ItemSpendPercent;
  UPROPERTY(EditAnywhere)
  TMap<EItemWealthType, int32> ItemNeeds;

  UPROPERTY(EditAnywhere)
  FCustomerPopAssetData AssetData;
};

// * Dynamic data for each customer pop.
USTRUCT()
struct FPopEconData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  FName PopID;

  UPROPERTY(EditAnywhere, SaveGame)
  int32 Population;
  UPROPERTY(EditAnywhere, SaveGame)
  float Money;

  UPROPERTY(EditAnywhere, SaveGame)
  float GoodsBoughtPerCapita;  // * Total goods bought / population.
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<EItemWealthType, float> ItemNeedsFulfilled;
};

USTRUCT()
struct FEconItem {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  FName ItemID;

  UPROPERTY(EditAnywhere)
  EItemType ItemType;
  UPROPERTY(EditAnywhere)
  EItemEconType ItemEconType;
  UPROPERTY(EditAnywhere)
  EItemWealthType ItemWealthType;

  UPROPERTY(EditAnywhere, SaveGame)
  float CurrentPrice;  // * Trends towards perfect price.

  UPROPERTY(EditAnywhere)
  float PerfectPrice;  // * Price given current supply/demand pressure.
  UPROPERTY(EditAnywhere)
  float BoughtToPriceMulti;  // * Determine price given amount bought. Calculated once at game start.

  UPROPERTY(EditAnywhere)
  float PriceJumpMulti;  // * Reverse price stickiness, 1.0 = current price changes to perfect price in one cycle.
};

// * Store types.
USTRUCT()
struct FNpcStoreAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  UTexture2D* Avatar;
  UPROPERTY(EditAnywhere)
  UStaticMesh* Mesh;
  UPROPERTY(EditAnywhere)
  TMap<ESimpleSpriteDirection, UPaperFlipbook*> Sprites;
};

USTRUCT()
struct FNpcStoreType {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  FText StoreTypeName;
  UPROPERTY(EditAnywhere)
  int32 StoreSpawnWeight;
  UPROPERTY(EditAnywhere)
  TArray<int32> StockCountRange;  // * Min, max stock count.

  UPROPERTY(EditAnywhere)
  float StoreMarkup;

  UPROPERTY(EditAnywhere)
  TMap<EItemType, float> ItemTypeWeightMap;  // * Item types sold, weighted.
  UPROPERTY(EditAnywhere)
  TMap<EItemEconType, float> ItemEconTypeWeightMap;  // * Item econ types sold, weighted.

  UPROPERTY(EditAnywhere)
  FNpcStoreAssetData AssetData;
};
USTRUCT()
struct FNpcStoreTypeRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  FText StoreTypeName;
  UPROPERTY(EditAnywhere)
  int32 StoreSpawnWeight;
  UPROPERTY(EditAnywhere)
  TArray<int32> StockCountRange;  // * Min, max stock count.

  UPROPERTY(EditAnywhere)
  float StoreMarkup;

  UPROPERTY(EditAnywhere)
  TMap<EItemType, float> ItemTypeWeightMap;  // * Item types sold, weighted.
  UPROPERTY(EditAnywhere)
  TMap<EItemEconType, float> ItemEconTypeWeightMap;  // * Item econ types sold, weighted.

  UPROPERTY(EditAnywhere)
  FNpcStoreAssetData AssetData;
};

// * Economic events.
USTRUCT()
struct FPriceEffect {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  FName ID;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<EItemEconType> ItemEconTypes;
  UPROPERTY(EditAnywhere, SaveGame)
  TArray<EItemWealthType> ItemWealthTypes;
  UPROPERTY(EditAnywhere, SaveGame)
  TArray<EItemType> ItemTypes;

  UPROPERTY(EditAnywhere, SaveGame)
  float PriceMultiPercent;  // * Positive and negative, additive.

  UPROPERTY(EditAnywhere, SaveGame)
  int32 DurationLeft;  // * Duration in days.
  UPROPERTY(EditAnywhere, SaveGame)
  float PriceMultiPercentFalloff;  // * Falloff each day, if any.
};
USTRUCT()
struct FPriceEffectRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  TArray<EItemEconType> ItemEconTypes;
  UPROPERTY(EditAnywhere)
  TArray<EItemWealthType> ItemWealthTypes;
  UPROPERTY(EditAnywhere)
  TArray<EItemType> ItemTypes;

  UPROPERTY(EditAnywhere)
  float PriceMultiPercent;  // * Positive and negative, additive.

  UPROPERTY(EditAnywhere)
  int32 Duration;  // * Duration in days.
  UPROPERTY(EditAnywhere)
  float PriceMultiPercentFalloff;  // * Falloff each day, if any.
};

USTRUCT()
struct FPopEffect {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  FName ID;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<EPopType> PopTypes;
  UPROPERTY(EditAnywhere, SaveGame)
  TArray<EPopWealthType> PopWealthTypes;

  UPROPERTY(EditAnywhere, SaveGame)
  float PopChangeMulti;  // * Chance to change pop.

  UPROPERTY(EditAnywhere, SaveGame)
  int32 DurationLeft;  // * Duration in days.
  UPROPERTY(EditAnywhere, SaveGame)
  float PopChangeMultiFalloff;  // * Falloff each day, if any.
};
USTRUCT()
struct FPopEffectRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  FName ID;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<EPopType> PopTypes;
  UPROPERTY(EditAnywhere, SaveGame)
  TArray<EPopWealthType> PopWealthTypes;

  UPROPERTY(EditAnywhere, SaveGame)
  float PopChangeMulti;  // * Chance to change pop.

  UPROPERTY(EditAnywhere, SaveGame)
  int32 Duration;  // * Duration in days.
  UPROPERTY(EditAnywhere, SaveGame)
  float PopChangeMultiFalloff;  // * Falloff each day, if any.
};

UENUM()
enum class EEconEventSeverity : uint8 {
  Minor UMETA(DisplayName = "Minor"),
  Major UMETA(DisplayName = "Major"),
  Catastrophic UMETA(DisplayName = "Catastrophic"),  // * Doesn't mean bad, just very impactful.
};

USTRUCT()
struct FEconEventTextData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FText Name;
  UPROPERTY(EditAnywhere, meta = (MultiLine = true))
  FText Description;
};
USTRUCT()
struct FEconEventAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  class UTexture2D* Icon;
};

USTRUCT()
struct FEconEvent {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  EEconEventSeverity Severity;  // * Determines how the world reacts to the event.

  UPROPERTY(EditAnywhere)
  bool bIsUnlocked;
  UPROPERTY(EditAnywhere)
  FName RequirementsFilter;
  UPROPERTY(EditAnywhere)
  float StartChance;
  UPROPERTY(EditAnywhere)
  bool bIsRepeatable;

  UPROPERTY(EditAnywhere)
  TArray<FName> PriceEffectIDs;
  UPROPERTY(EditAnywhere)
  TArray<FName> PopEffectIDs;
  UPROPERTY(EditAnywhere)
  FName ArticleID;  // * Linked article that will guarantee appear, if any.

  UPROPERTY(EditAnywhere)
  FEconEventTextData TextData;
  UPROPERTY(EditAnywhere)
  FEconEventAssetData AssetData;

  UPROPERTY(EditAnywhere, SaveGame)
  FGameplayTagContainer Tags;  // * Tags for systems to filter further.
};
USTRUCT()
struct FEconEventRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  EEconEventSeverity Severity;  // * Determines how the world reacts to the event.

  UPROPERTY(EditAnywhere)
  bool bIsUnlockable;
  UPROPERTY(EditAnywhere)
  FName RequirementsFilter;
  UPROPERTY(EditAnywhere)
  float StartChance;
  UPROPERTY(EditAnywhere)
  bool bIsRepeatable;

  UPROPERTY(EditAnywhere)
  TArray<FName> PriceEffectIDs;
  UPROPERTY(EditAnywhere)
  TArray<FName> PopEffectIDs;
  UPROPERTY(EditAnywhere)
  FName ArticleID;  // * Linked article that will guarantee appear, if any.

  UPROPERTY(EditAnywhere)
  FEconEventTextData TextData;
  UPROPERTY(EditAnywhere)
  FEconEventAssetData AssetData;

  UPROPERTY(EditAnywhere, SaveGame)
  FGameplayTagContainer Tags;  // * Tags for systems to filter further.
};