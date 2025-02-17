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

void UNegotiationSystem::RequestNegotiation() {
  ENegotiationState NextState = GetNextNegotiationState(NegotiationState, ENegotiationAction::NPCRequest);
  if (NextState == ENegotiationState::None) return;

  NegotiationState = NextState;
}

void UNegotiationSystem::Consider(Negotiator CallingNegotiator) {
  ENegotiationState NextState = GetNextNegotiationState(NegotiationState, ENegotiationAction::Consider);
  if (NextState == ENegotiationState::None) return;

  NegotiationState = NextState;

  // ? Move call to somewhere else?
  if (NegotiationState != ENegotiationState::NPCConsider) return;
  NPCNegotiationTurn();
}

void UNegotiationSystem::OfferPrice(Negotiator CallingNegotiator, float Price) {
  ENegotiationState NextState = GetNextNegotiationState(NegotiationState, ENegotiationAction::OfferPrice);
  if (NextState == ENegotiationState::None) return;

  NegotiationState = NextState;
  OfferedPrice = Price;
}

void UNegotiationSystem::AcceptOffer(Negotiator CallingNegotiator) {
  ENegotiationState NextState = GetNextNegotiationState(NegotiationState, ENegotiationAction::Accept);
  if (NextState == ENegotiationState::None) return;

  NegotiationState = NextState;

  // Note: Move to actual object (player, npc) if fine grain functionality needed.
  if (NegotiationState != ENegotiationState::Accepted) return;
  PlayerInventory->AddItem(NegotiatedItem, Quantity);
}

void UNegotiationSystem::RejectOffer(Negotiator CallingNegotiator) {
  ENegotiationState NextState = GetNextNegotiationState(NegotiationState, ENegotiationAction::Reject);
  if (NextState == ENegotiationState::None) return;

  NegotiationState = NextState;
}

void UNegotiationSystem::NPCNegotiationTurn() {
  OfferResponse Response = NegotiationAI->ConsiderOffer(BasePrice, OfferedPrice);

  if (Response.Accepted) {
    AcceptOffer(Negotiator::NPC);
    return;
  }

  OfferPrice(Negotiator::NPC, Response.CounterOffer);
}