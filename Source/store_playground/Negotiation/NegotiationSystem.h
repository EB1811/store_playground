// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include "CoreMinimal.h"
#include "NegotiationSystem.generated.h"

UENUM()
enum class Negotiator : uint8 {
  PLAYER,
  NPC,
};
UENUM()
enum class ENegotiationState : uint8 {
  NONE UMETA(DisplayName = "NONE"),
  NPC_TURN UMETA(DisplayName = "NPC Turn"),
  PLAYER_TURN UMETA(DisplayName = "Player Turn"),
  ACCEPTED UMETA(DisplayName = "Accepted"),
  REJECTED UMETA(DisplayName = "Rejected")
};
UENUM()
enum class ENegotiationAction : uint8 {
  OFFER UMETA(DisplayName = "Offer"),
  ACCEPT UMETA(DisplayName = "Accept"),
  REJECT UMETA(DisplayName = "Reject")
};

UCLASS()
class STORE_PLAYGROUND_API UNegotiationSystem : public UObject {
  GENERATED_BODY()

public:
  UNegotiationSystem() : NegotiationState(ENegotiationState::NONE), BasePrice(0), OfferedPrice(0) {}

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
                        ENegotiationState InitState = ENegotiationState::PLAYER_TURN);
  void OfferPrice(Negotiator CallingNegotiator, float Price);
  void AcceptOffer(Negotiator CallingNegotiator);
  void RejectOffer(Negotiator CallingNegotiator);

  void NPCNegotiationTurn();
};

extern std::map<ENegotiationState, FText> NegotiationStateToName;
ENegotiationState GetNextNegotiationState(ENegotiationState State, ENegotiationAction Action);
