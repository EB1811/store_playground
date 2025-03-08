#include "NegotiationSystem.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/AI/NegotiationAI.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"

void UNegotiationSystem::StartNegotiation(const UItemBase* Item,
                                          UCustomerAIComponent* _CustomerAI,
                                          UInventoryComponent* _FromInventory,
                                          float Price,
                                          ENegotiationState InitState) {
  NegotiatedItems.Empty();
  NegotiatedItems.Add(Item);
  Type = _CustomerAI->NegotiationAI->RequestType == ECustomerRequestType::SellItem ? NegotiationType::PlayerBuy
                                                                                   : NegotiationType::PlayerSell;
  Quantity = 1;
  CustomerAI = _CustomerAI;
  FromInventory = _FromInventory;
  BasePrice = Price;
  OfferedPrice = BasePrice;
  NegotiationState = InitState;

  CustomerAI->StartNegotiation();
  DialogueSystem->ResetDialogue();
}

// ? Turn FNextDialogueRes into using the dialogue system directly in the ui?
FNextDialogueRes UNegotiationSystem::NPCRequestNegotiation() {
  if (CustomerAI->NegotiationAI->RequestType == ECustomerRequestType::StockCheck) {
    NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::NpcStockCheckRequest);
    return DialogueSystem->StartDialogue(
        CustomerAI->NegotiationAI->DialoguesMap[ENegotiationDialogueType::StockCheckRequest].Dialogues);
  }

  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::NpcRequest);
  return DialogueSystem->StartDialogue(
      CustomerAI->NegotiationAI->DialoguesMap[ENegotiationDialogueType::Request].Dialogues);
}

void UNegotiationSystem::PlayerReadRequest() {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::PlayerReadRequest);
}

void UNegotiationSystem::PlayerShowItem(UItemBase* Item, UInventoryComponent* _FromInventory) {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::PlayerShowItem);

  check(Item && _FromInventory);

  CustomerOfferResponse = CustomerAI->NegotiationAI->ConsiderStockCheck(Item);
  NegotiatedItems.Empty();
  NegotiatedItems.Add(Item);
  FromInventory = _FromInventory;
}

FNextDialogueRes UNegotiationSystem::NPCNegotiationTurn() {
  switch (NegotiationState) {
    case ENegotiationState::NpcStockCheckConsider: {
      if (CustomerOfferResponse.Accepted)
        NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::Accept);
      else
        NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::Reject);

      return DialogueSystem->StartDialogue(CustomerOfferResponse.ResponseDialogue);
    }
    case ENegotiationState::NpcConsider: {
      if (CustomerOfferResponse.Accepted)
        AcceptOffer();
      else
        OfferPrice(CustomerOfferResponse.CounterOffer);

      return DialogueSystem->StartDialogue(CustomerOfferResponse.ResponseDialogue);
    }
    default: checkf(false, TEXT("Invalid State %d"), (int)NegotiationState); return {};
  }
}

void UNegotiationSystem::OfferPrice(float Price) {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::OfferPrice);

  if (NegotiationState == ENegotiationState::NpcConsider)
    CustomerOfferResponse = CustomerAI->NegotiationAI->ConsiderOffer(Type == NegotiationType::PlayerSell ? true : false,
                                                                     BasePrice, OfferedPrice, Price);

  OfferedPrice = Price;
}

void UNegotiationSystem::AcceptOffer() {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::Accept);

  if (NegotiationState == ENegotiationState::Accepted) NegotiationSuccess();
}

void UNegotiationSystem::RejectOffer() {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::Reject);

  if (NegotiationState == ENegotiationState::Rejected) NegotiationFailure();
}

// Note: Move to actual object (player, npc) if fine grain functionality needed.
void UNegotiationSystem::NegotiationSuccess() {
  if (Type == NegotiationType::PlayerSell) {
    check(FromInventory);

    for (const UItemBase* NegotiatedItem : NegotiatedItems) FromInventory->RemoveItem(NegotiatedItem);
    Store->Money += OfferedPrice;
    Store->StoreStockItems.RemoveAll(
        [this](const FStockItem& StockItem) { return NegotiatedItems.Contains(StockItem.Item); });
  } else {
    check(PlayerInventory);

    for (const UItemBase* NegotiatedItem : NegotiatedItems) PlayerInventory->AddItem(NegotiatedItem);
    Store->Money -= OfferedPrice;
  }

  CustomerAI->PostNegotiation();

  NegotiatedItems.Empty();
  CustomerAI = nullptr;
  FromInventory = nullptr;
}

void UNegotiationSystem::NegotiationFailure() {
  CustomerAI->PostNegotiation();

  NegotiatedItems.Empty();
  CustomerAI = nullptr;
  FromInventory = nullptr;
}