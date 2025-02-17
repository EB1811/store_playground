#include "NegotiationSystem.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/AI/NegotiationAI.h"

void UNegotiationSystem::StartNegotiation(const UItemBase* Item,
                                          const UNegotiationAI* _NegotiationAI,
                                          UInventoryComponent* _PlayerInventory,
                                          float Price,
                                          ENegotiationState InitState) {
  NegotiationState = InitState;
  BasePrice = Price;

  NegotiatedItem = Item;
  Quantity = Item->Quantity;
  NegotiationAI = _NegotiationAI;
  PlayerInventory = _PlayerInventory;
}

// ? Change to just have different actions and use state machine to determine if the action is valid?
void UNegotiationSystem::OfferPrice(Negotiator CallingNegotiator, float Price) {
  if (CallingNegotiator == Negotiator::Player && NegotiationState != ENegotiationState::PlayerTurn) return;
  if (CallingNegotiator == Negotiator::NPC && NegotiationState != ENegotiationState::NPCTurn) return;

  OfferedPrice = Price;

  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::Offer);

  // ? Move call to somewhere else?
  if (NegotiationState != ENegotiationState::NPCTurn) return;
  NPCNegotiationTurn();
}

void UNegotiationSystem::AcceptOffer(Negotiator CallingNegotiator) {
  if (CallingNegotiator == Negotiator::Player && NegotiationState != ENegotiationState::PlayerTurn) return;
  if (CallingNegotiator == Negotiator::NPC && NegotiationState != ENegotiationState::NPCTurn) return;

  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::Accept);

  // Note: Move to actual object (player, npc) if fine grain functionality needed.
  if (PlayerInventory) PlayerInventory->AddItem(NegotiatedItem, Quantity);
}

void UNegotiationSystem::RejectOffer(Negotiator CallingNegotiator) {
  if (CallingNegotiator == Negotiator::NPC && NegotiationState != ENegotiationState::NPCTurn) return;

  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::Reject);
}

void UNegotiationSystem::NPCNegotiationTurn() {
  OfferResponse Response = NegotiationAI->ConsiderOffer(BasePrice, OfferedPrice);

  if (Response.Accepted) {
    AcceptOffer(Negotiator::NPC);
    return;
  }

  OfferPrice(Negotiator::NPC, Response.CounterOffer);
}