// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/SlateEnums.h"
#include "store_playground/AI/CustomerDataStructs.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
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

USTRUCT()
struct FNegotiationAIDetails {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FText CustomerName;
  UPROPERTY(EditAnywhere)
  TMap<ENegotiationDialogueType, FDialoguesArray> DialoguesMap;

  UPROPERTY(EditAnywhere)
  ECustomerRequestType RequestType;

  UPROPERTY(EditAnywhere)
  class UItemBase* RelevantItem;
  UPROPERTY(EditAnywhere)
  class UInventoryComponent* StockDisplayInventory;

  UPROPERTY(EditAnywhere)
  FWantedItemType WantedItemType;

  UPROPERTY(EditAnywhere)
  float MoneyToSpend;
  UPROPERTY(EditAnywhere)
  float AcceptancePercentage;  // * + or - depending on whether NPC is buying or selling.
  UPROPERTY(EditAnywhere)
  float AcceptanceFalloffMulti;  // * The rate of which acceptance falls off using OfferedPercentBetween.
  UPROPERTY(EditAnywhere)
  float HagglingCount;  // * Decreases after each haggling attempt.
};