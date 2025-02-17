// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <array>
#include <map>
#include "CoreMinimal.h"
#include "NegotiationSystem.generated.h"

UENUM()
enum class Negotiator : uint8 {
  Player,
  NPC,
};
UENUM()
enum class ENegotiationState : uint8 {
  None UMETA(DisplayName = "NONE"),
  NPCRequest UMETA(DisplayName = "NPC Request"),
  PlayerOffer UMETA(DisplayName = "Player Offer"),
  NPCConsider UMETA(DisplayName = "NPC Consider"),
  Accepted UMETA(DisplayName = "Accepted"),
  Rejected UMETA(DisplayName = "Rejected")
};
UENUM()
enum class ENegotiationAction : uint8 {
  NPCRequest UMETA(DisplayName = "NPC Request"),
  Consider UMETA(DisplayName = "Consider"),
  OfferPrice UMETA(DisplayName = "Offer Price"),
  Accept UMETA(DisplayName = "Accept"),
  Reject UMETA(DisplayName = "Reject")
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API UNegotiationSystem : public UObject {
  GENERATED_BODY()

public:
  UNegotiationSystem() : NegotiationState(ENegotiationState::None), BasePrice(0), OfferedPrice(0) {}

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  ENegotiationState NegotiationState;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  float BasePrice;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  float OfferedPrice;

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  const class UItemBase* NegotiatedItem;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  int16 Quantity;

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  const class UNegotiationAI* NegotiationAI;

  // Temp: Just inventory for now.
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  class UInventoryComponent* PlayerInventory;

  void StartNegotiation(const class UItemBase* NegotiatedItem,
                        const class UNegotiationAI* _NegotiationAI,
                        class UInventoryComponent* _PlayerInventory,
                        float BasePrice,
                        ENegotiationState InitState = ENegotiationState::NPCRequest);
  void RequestNegotiation();
  void Consider(Negotiator CallingNegotiator);
  void OfferPrice(Negotiator CallingNegotiator, float Price);
  void AcceptOffer(Negotiator CallingNegotiator);
  void RejectOffer(Negotiator CallingNegotiator);

  void NPCNegotiationTurn();
};

extern std::map<ENegotiationState, FText> NegotiationStateToName;
ENegotiationState GetNextNegotiationState(ENegotiationState State, ENegotiationAction Action);
