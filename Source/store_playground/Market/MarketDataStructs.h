// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "store_playground/Item/ItemDataStructs.h"
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
UENUM()
enum class EPopWealthType : uint8 {
  Poorer UMETA(DisplayName = "Poorer"),
  Middle UMETA(DisplayName = "Middle"),
  Upper UMETA(DisplayName = "Upper"),
};

USTRUCT()
struct FCustomerPopDataRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FText PopName;

  UPROPERTY(EditAnywhere)
  EPopType PopType;
  UPROPERTY(EditAnywhere)
  EPopWealthType WealthType;
  UPROPERTY(EditAnywhere)
  TArray<EItemEconType> ItemEconTypes;
  UPROPERTY(EditAnywhere)
  int32 InitPopulation;
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

USTRUCT()
struct FPopEconData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FGuid PopID;

  UPROPERTY(EditAnywhere)
  int32 Population;
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
  FGuid PopID;
  UPROPERTY(EditAnywhere)
  FText PopName;

  UPROPERTY(EditAnywhere)
  EPopType PopType;
  UPROPERTY(EditAnywhere)
  EPopWealthType WealthType;
  UPROPERTY(EditAnywhere)
  TArray<EItemEconType> ItemEconTypes;

  UPROPERTY(EditAnywhere)
  FPopEconData EconData;
};

USTRUCT()
struct FPopEconGenData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FGuid PopID;

  UPROPERTY(EditAnywhere)
  int32 Population;

  UPROPERTY(EditAnywhere)
  int32 MGen;
  UPROPERTY(EditAnywhere)
  float MSharePercent;
};

USTRUCT()
struct FPopMoneySpendData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FGuid PopID;

  UPROPERTY(EditAnywhere)
  float Money;
  UPROPERTY(EditAnywhere)
  int32 Population;

  UPROPERTY(EditAnywhere)
  TMap<EItemWealthType, float> ItemSpendPercent;
  UPROPERTY(EditAnywhere)
  TMap<EItemWealthType, int32> ItemNeeds;
  UPROPERTY(EditAnywhere)
  TMap<EItemWealthType, float> ItemNeedsFulfilled;
};

USTRUCT()
struct FEconItem {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ItemID;
  UPROPERTY(EditAnywhere)
  EItemWealthType ItemWealthType;

  UPROPERTY(EditAnywhere)
  float CurrentPrice;  // * Trends towards perfect price.

  UPROPERTY(EditAnywhere)
  float PerfectPrice;  // * Price given current supply/demand pressure.
  UPROPERTY(EditAnywhere)
  float BoughtToPriceMulti;  // * Determine price given amount bought. Calculated once at game start.

  UPROPERTY(EditAnywhere)
  float PriceJumpMulti;  // * Reverse price stickiness, 1.0 = current price changes to perfect price in one cycle.
};