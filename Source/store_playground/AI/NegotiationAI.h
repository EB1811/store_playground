// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include "CoreMinimal.h"
#include "NegotiationAI.generated.h"

UENUM()
enum class ENegotiatorAttitude : uint8 {
  FRIENDLY,
  NEUTRAL,
  HOSTILE,
};

struct OfferResponse {
  bool Accepted;
  float CounterOffer;
};

UCLASS()
class STORE_PLAYGROUND_API UNegotiationAI : public UObject {
  GENERATED_BODY()
public:
  UNegotiationAI() : Attitude(ENegotiatorAttitude::NEUTRAL) {}

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  ENegotiatorAttitude Attitude;

  OfferResponse ConsiderOffer(float BasePrice, float OfferedPrice) const;
};

// Temp: Make into function using random.
extern std::map<ENegotiatorAttitude, float> AttitudeToPercentAcceptance;