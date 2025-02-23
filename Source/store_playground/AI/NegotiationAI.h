// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include "CoreMinimal.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "NegotiationAI.generated.h"

UENUM()
enum class ENegotiatorAttitude : uint8 {
  FRIENDLY,
  NEUTRAL,
  HOSTILE,
};

USTRUCT()
struct FOfferResponse {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float CounterOffer;
  UPROPERTY(EditAnywhere)
  bool Accepted;
};

UCLASS()
class STORE_PLAYGROUND_API UNegotiationAI : public UObject {
  GENERATED_BODY()
public:
  UNegotiationAI() : Attitude(ENegotiatorAttitude::NEUTRAL) {}

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  ENegotiatorAttitude Attitude;

  // Temp: Use pointer instead of copy.
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TMap<ENegotiationDialogueType, struct FDialogueData> DialogueMap;

  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TArray<struct FDialogueData> RequestDialogueArray;

  FOfferResponse ConsiderOffer(float BasePrice, float LastOfferedPrice, float PlayerOfferedPrices) const;
};

// Temp: Make into function using random.
extern std::map<ENegotiatorAttitude, float> AttitudeToPercentAcceptance;