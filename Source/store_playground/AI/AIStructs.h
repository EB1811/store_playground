// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/SlateEnums.h"
#include "AIStructs.generated.h"

UENUM()
enum class ECustomerAction : uint8 {
  None UMETA(DisplayName = "NONE"),
  PickItem UMETA(DisplayName = "PickItem"),
  StockCheck UMETA(DisplayName = "StockCheck"),
  SellItem UMETA(DisplayName = "SellItem"),
  Leave UMETA(DisplayName = "Leave"),
};
ENUM_RANGE_BY_COUNT(ECustomerAction, 5);

UENUM()
enum class ECustomerRequestType : uint8 {
  BuyStockItem UMETA(DisplayName = "Buy Stock Item"),
  SellItem UMETA(DisplayName = "Sell Item"),
  StockCheck UMETA(DisplayName = "Stock Check"),
};

USTRUCT()
struct FOfferResponse {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  bool Accepted;
  UPROPERTY(EditAnywhere)
  float CounterOffer;
  UPROPERTY(EditAnywhere)
  TArray<struct FDialogueData> ResponseDialogue;
  UPROPERTY(EditAnywhere)
  FText CustomerName;
};