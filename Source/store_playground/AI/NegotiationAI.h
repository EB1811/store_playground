// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include "CoreMinimal.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/AI/CustomerDataStructs.h"
#include "store_playground/AI/AIStructs.h"
#include "NegotiationAI.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UNegotiationAI : public UObject {
  GENERATED_BODY()
public:
  UNegotiationAI() {}

  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  FText CustomerName;
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  TMap<ENegotiationDialogueType, FDialoguesArray> DialoguesMap;

  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  ECustomerRequestType RequestType;

  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  class UItemBase* RelevantItem;
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  class UInventoryComponent* StockDisplayInventory;

  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  FWantedItemType WantedItemType;

  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  float MoneyToSpend;
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  float AcceptancePercentage;  // * + or - depending on whether NPC is buying or selling.
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  float AcceptanceFalloffMulti;  // * The rate of which acceptance falls off using OfferedPercentBetween.
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  float HagglingCount;  // * Decreases after each haggling attempt.

  // FOfferResponse ConsiderOffer(bool bNpcBuying,
  //                              float MarketPrice,
  //                              float LastOfferedPrice,
  //                              float PlayerOfferedPrices) const;
  // FOfferResponse ConsiderStockCheck(const class UItemBase* Item, float MarketPrice) const;
};