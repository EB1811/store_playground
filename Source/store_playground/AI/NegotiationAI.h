// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include "CoreMinimal.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/AI/CustomerDataStructs.h"
#include "NegotiationAI.generated.h"

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
  TArray<struct FDialogueData> ResponseDialogue;
};

UCLASS()
class STORE_PLAYGROUND_API UNegotiationAI : public UObject {
  GENERATED_BODY()
public:
  UNegotiationAI() {}

  TMap<ENegotiationDialogueType, FDialoguesArray> DialoguesMap;

  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  ECustomerRequestType RequestType;

  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  const class UItemBase* RelevantItem;
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  class UInventoryComponent* StockDisplayInventory;

  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  FWantedItemType WantedItemType;

  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  float AcceptancePercentage;
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  float MaxHagglingCount;

  FOfferResponse ConsiderOffer(bool bNpcBuying,
                               float MarketPrice,
                               float LastOfferedPrice,
                               float PlayerOfferedPrices) const;
  FOfferResponse ConsiderStockCheck(class UItemBase* Item) const;
};