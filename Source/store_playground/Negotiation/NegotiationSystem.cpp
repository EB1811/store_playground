#include "NegotiationSystem.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/AI/NegotiationAI.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/Store/Store.h"

void UNegotiationSystem::StartNegotiation(const UItemBase* Item,
                                          bool bNpcBuying,
                                          UCustomerAIComponent* _CustomerAI,
                                          UInventoryComponent* _FromInventory,
                                          float Price,
                                          ENegotiationState InitState) {
  NegotiatedItem = Item;
  Type = bNpcBuying ? NegotiationType::PlayerSell : NegotiationType::PlayerBuy;
  Quantity = Item->Quantity;
  CustomerAI = _CustomerAI;
  FromInventory = _FromInventory;
  BasePrice = Price;
  OfferedPrice = BasePrice;
  NegotiationState = InitState;

  CustomerAI->StartNegotiation();
  DialogueSystem->ResetDialogue();
}

FNextDialogueRes UNegotiationSystem::NPCRequestNegotiation() {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::NPCRequest);

  return DialogueSystem->StartDialogue(CustomerAI->NegotiationAI->RequestDialogueArray);
}

void UNegotiationSystem::PlayerReadRequest() {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::PlayerReadRequest);
}

FNextDialogueRes UNegotiationSystem::NPCNegotiationTurn() {
  if (CustomerOfferResponse.Accepted)
    AcceptOffer(Negotiator::NPC);
  else
    OfferPrice(Negotiator::NPC, CustomerOfferResponse.CounterOffer);

  return DialogueSystem->StartDialogue(CustomerOfferResponse.ResponseDialogue);
}

void UNegotiationSystem::OfferPrice(Negotiator CallingNegotiator, float Price) {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::OfferPrice);

  if (NegotiationState == ENegotiationState::NPCConsider)
    CustomerOfferResponse = CustomerAI->NegotiationAI->ConsiderOffer(Type == NegotiationType::PlayerSell ? true : false,
                                                                     BasePrice, OfferedPrice, Price);

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
void UNegotiationSystem::NegotiationSuccess() {
  if (Type == NegotiationType::PlayerSell) {
    FromInventory->RemoveItem(NegotiatedItem, Quantity);
    Store->Money += OfferedPrice * Quantity;
  } else {
    FromInventory->AddItem(NegotiatedItem, Quantity);
    Store->Money -= OfferedPrice * Quantity;
  }
  UE_LOG(LogTemp, Warning, TEXT("Money: %f"), Store->Money);

  CustomerAI->PostNegotiation();

  NegotiatedItem = nullptr;
  CustomerAI = nullptr;
  FromInventory = nullptr;
}

void UNegotiationSystem::NegotiationFailure() {
  CustomerAI->PostNegotiation();

  NegotiatedItem = nullptr;
  CustomerAI = nullptr;
  FromInventory = nullptr;
}