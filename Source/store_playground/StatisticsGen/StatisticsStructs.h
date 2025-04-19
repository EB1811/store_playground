// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "StatisticsStructs.generated.h"

USTRUCT()
struct FItemDeal {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  FName ItemId;

  UPROPERTY(EditAnywhere, SaveGame)
  float BoughtAt;
  UPROPERTY(EditAnywhere, SaveGame)
  float SoldAt;

  UPROPERTY(EditAnywhere, SaveGame)
  int32 Quantity;
};
USTRUCT()
struct FStoreMoneyActivity {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  float AllExpenses;
  UPROPERTY(EditAnywhere, SaveGame)
  float AllIncome;
};