// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include "CoreMinimal.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
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
  float CounterOffer;
  UPROPERTY(EditAnywhere)
  bool Accepted;
  TArray<struct FDialogueData> ResponseDialogue;
};

UCLASS()
class STORE_PLAYGROUND_API UNegotiationAI : public UObject {
  GENERATED_BODY()
public:
  UNegotiationAI() {}

  // TODO: Change to map.
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  TArray<struct FDialogueData> RequestDialogueArray;
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  TArray<struct FDialogueData> ConsiderTooHighArray;
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  TArray<struct FDialogueData> ConsiderCloseArray;
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  TArray<struct FDialogueData> AcceptArray;
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  TArray<struct FDialogueData> RejectArray;

  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  ECustomerRequestType RequestType;
  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  const class UItemBase* RelevantItem;

  UPROPERTY(EditAnywhere, Category = "Negotiation AI")
  float AcceptancePercentage;

  FOfferResponse ConsiderOffer(bool bNpcBuying,
                               float BasePrice,
                               float LastOfferedPrice,
                               float PlayerOfferedPrices) const;
  FOfferResponse ConsiderStockCheck(class UItemBase* Item) const;
};