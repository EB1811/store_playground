#include "NegotiationSystem.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/AI/NegotiationAI.h"
#include "store_playground/Dialogue/DialogueSystem.h"

void UNegotiationSystem::StartNegotiation(const UItemBase* Item,
                                          const UNegotiationAI* _NegotiationAI,
                                          UDialogueSystem* _DialogueSystem,
                                          UInventoryComponent* _PlayerInventory,
                                          float Price,
                                          ENegotiationState InitState) {
  NegotiatedItem = Item;
  Quantity = Item->Quantity;
  NegotiationAI = _NegotiationAI;
  PlayerInventory = _PlayerInventory;
  DialogueSystem = _DialogueSystem;
  BasePrice = Price;
  OfferedPrice = BasePrice;
  NegotiationState = InitState;
}

FNextDialogueRes UNegotiationSystem::NPCRequestNegotiation() {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::NPCRequest);

  return DialogueSystem->StartDialogue(NegotiationAI->RequestDialogueArray);
}

void UNegotiationSystem::PlayerReadRequest() {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::PlayerReadRequest);
}

FNextDialogueRes UNegotiationSystem::NPCNegotiationTurn() {
  if (OfferResponse.Accepted)
    AcceptOffer(Negotiator::NPC);
  else
    OfferPrice(Negotiator::NPC, OfferResponse.CounterOffer);

  return DialogueSystem->StartDialogue(NegotiationAI->RequestDialogueArray);
}

void UNegotiationSystem::OfferPrice(Negotiator CallingNegotiator, float Price) {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::OfferPrice);

  if (NegotiationState == ENegotiationState::NPCConsider)
    OfferResponse = NegotiationAI->ConsiderOffer(BasePrice, OfferedPrice, Price);

  OfferedPrice = Price;
}

void UNegotiationSystem::AcceptOffer(Negotiator CallingNegotiator) {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::Accept);

  if (NegotiationState == ENegotiationState::Accepted) NegotiationSuccess();
}

void UNegotiationSystem::RejectOffer(Negotiator CallingNegotiator) {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::Reject);

  if (NegotiationState == ENegotiationState::Rejected) NegotiationFailure();
}

// Note: Move to actual object (player, npc) if fine grain functionality needed.
void UNegotiationSystem::NegotiationSuccess() { PlayerInventory->AddItem(NegotiatedItem, Quantity); }

void UNegotiationSystem::NegotiationFailure() {}