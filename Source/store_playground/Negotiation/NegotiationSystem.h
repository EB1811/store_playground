// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <optional>
#include <array>
#include <map>
#include "CoreMinimal.h"
#include "store_playground/AI/NegotiationAI.h"
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
  PlayerConsider UMETA(DisplayName = "Player Consider"),
  NPCConsider UMETA(DisplayName = "NPC Consider"),
  Accepted UMETA(DisplayName = "Accepted"),
  Rejected UMETA(DisplayName = "Rejected")
};
UENUM()
enum class ENegotiationAction : uint8 {
  NPCRequest UMETA(DisplayName = "NPC Request"),
  PlayerReadRequest UMETA(DisplayName = "Player Read Request"),
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
  class UDialogueSystem* DialogueSystem;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  class AStore* Store;

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  ENegotiationState NegotiationState;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  float BasePrice;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  float OfferedPrice;

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  const class UItemBase* NegotiatedItem;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  int32 Quantity;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  class UInventoryComponent* FromInventory;
  UPROPERTY(EditAnywhere, Category = "Negotiation")
  class UCustomerAIComponent* CustomerAI;

  UPROPERTY(EditAnywhere, Category = "Negotiation")
  FOfferResponse CustomerOfferResponse;

  void StartNegotiation(const class UItemBase* NegotiatedItem,
                        class UCustomerAIComponent* _CustomerAI,
                        class UInventoryComponent* _FromInventory,
                        float BasePrice,
                        ENegotiationState InitState = ENegotiationState::None);

  struct FNextDialogueRes NPCRequestNegotiation();
  void PlayerReadRequest();
  struct FNextDialogueRes NPCNegotiationTurn();
  void OfferPrice(Negotiator CallingNegotiator, float Price);
  void AcceptOffer(Negotiator CallingNegotiator);
  void RejectOffer(Negotiator CallingNegotiator);

  void NegotiationSuccess();
  void NegotiationFailure();
};

extern std::map<ENegotiationState, FText> NegotiationStateToName;
ENegotiationState GetNextNegotiationState(ENegotiationState State, ENegotiationAction Action);
