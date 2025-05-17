#include "NegotiationSystem.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/AI/NegotiationAI.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Quest/QuestManager.h"
#include "store_playground/Quest/QuestComponent.h"

UNegotiationSystem::UNegotiationSystem() {
  NStateTransitions = {
      FNStateAction{ENegotiationState::None, ENegotiationAction::OfferPrice, ENegotiationState::PlayerConsider},
      FNStateAction{ENegotiationState::None, ENegotiationAction::NpcRequest, ENegotiationState::NpcRequest},
      FNStateAction{ENegotiationState::None, ENegotiationAction::NpcStockCheckRequest,
                    ENegotiationState::NpcStockCheckRequest},

      FNStateAction{ENegotiationState::NpcRequest, ENegotiationAction::PlayerReadRequest,
                    ENegotiationState::PlayerConsider},

      // * Alternatively, stock check.
      FNStateAction{ENegotiationState::NpcStockCheckRequest, ENegotiationAction::PlayerReadRequest,
                    ENegotiationState::PlayerStockCheck},

      FNStateAction{ENegotiationState::PlayerStockCheck, ENegotiationAction::PlayerShowItem,
                    ENegotiationState::NpcStockCheckConsider},
      FNStateAction{ENegotiationState::PlayerStockCheck, ENegotiationAction::Reject, ENegotiationState::Rejected},

      FNStateAction{ENegotiationState::NpcStockCheckConsider, ENegotiationAction::Accept,
                    ENegotiationState::PlayerConsider},
      FNStateAction{ENegotiationState::NpcStockCheckConsider, ENegotiationAction::Reject, ENegotiationState::Rejected},

      // * Negotiation back and forth.
      FNStateAction{ENegotiationState::PlayerConsider, ENegotiationAction::OfferPrice, ENegotiationState::NpcConsider},
      FNStateAction{ENegotiationState::PlayerConsider, ENegotiationAction::Accept, ENegotiationState::Accepted},
      FNStateAction{ENegotiationState::PlayerConsider, ENegotiationAction::Reject, ENegotiationState::Rejected},

      FNStateAction{ENegotiationState::NpcConsider, ENegotiationAction::OfferPrice, ENegotiationState::PlayerConsider},
      FNStateAction{ENegotiationState::NpcConsider, ENegotiationAction::Accept, ENegotiationState::Accepted},
      FNStateAction{ENegotiationState::NpcConsider, ENegotiationAction::Reject, ENegotiationState::Rejected},
  };
  BoughtAtPrice = 0;
  MarketPrice = 0;
  NegotiationState = ENegotiationState::None;
  OfferedPrice = 0;
  CustomerOfferResponse = {false, 0, {}};
  Type = NegotiationType::PlayerBuy;
}
ENegotiationState UNegotiationSystem::GetNextNegotiationState(ENegotiationState FNStateAction,
                                                              ENegotiationAction Action) {
  for (const struct FNStateAction& Transition : NStateTransitions)
    if (Transition.initial == FNStateAction && Transition.action == Action) return Transition.next;

  checkf(false, TEXT("Invalid Action %d for FNStateAction %d"), (int)Action, (int)FNStateAction);

  return ENegotiationState::None;
}

void UNegotiationSystem::StartNegotiation(UCustomerAIComponent* _CustomerAI,
                                          UItemBase* Item,
                                          UInventoryComponent* _FromInventory,
                                          bool _bIsQuestAssociated,
                                          UQuestComponent* _QuestComponent,
                                          ENegotiationState InitState) {
  NegotiatedItems.Empty();
  Type = _CustomerAI->NegotiationAI->RequestType == ECustomerRequestType::SellItem ? NegotiationType::PlayerBuy
                                                                                   : NegotiationType::PlayerSell;
  CustomerAI = _CustomerAI;
  bIsQuestAssociated = _bIsQuestAssociated;
  QuestComponent = _QuestComponent;
  NegotiationState = InitState;

  BoughtAtPrice = 0;
  MarketPrice = 0;
  OfferedPrice = 0;
  CustomerOfferResponse = {false, 0, {}};

  switch (CustomerAI->NegotiationAI->RequestType) {
    case ECustomerRequestType::StockCheck: WantedItemType = CustomerAI->NegotiationAI->WantedItemType; break;
    case ECustomerRequestType::BuyStockItem:
    case ECustomerRequestType::SellItem: {
      NegotiatedItems.Add(Item);
      FromInventory = _FromInventory;

      for (const UItemBase* NegotiatedItem : NegotiatedItems) {
        const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
            [NegotiatedItem](const FEconItem& EconItem) { return EconItem.ItemID == NegotiatedItem->ItemID; });
        check(EconItem);

        BoughtAtPrice += Item->PriceData.BoughtAt;
        MarketPrice += EconItem->CurrentPrice;
        OfferedPrice += MarketPrice;
      }
      break;
    }
    default: checkNoEntry(); break;
  }

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
  return Type == NegotiationType::PlayerSell
             ? DialogueSystem->StartDialogue(
                   CustomerAI->NegotiationAI->DialoguesMap[ENegotiationDialogueType::BuyItemRequest].Dialogues)
             : DialogueSystem->StartDialogue(
                   CustomerAI->NegotiationAI->DialoguesMap[ENegotiationDialogueType::SellItemRequest].Dialogues);
}

void UNegotiationSystem::PlayerReadRequest() {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::PlayerReadRequest);
}

// todo-low: Support multiple items.
void UNegotiationSystem::PlayerShowItem(UItemBase* Item, UInventoryComponent* _FromInventory) {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::PlayerShowItem);

  check(Item && _FromInventory);

  CustomerOfferResponse = CustomerAI->NegotiationAI->ConsiderStockCheck(Item);
  if (!CustomerOfferResponse.Accepted) return;

  NegotiatedItems.Empty();
  NegotiatedItems.Add(Item);
  FromInventory = _FromInventory;

  for (const UItemBase* NegotiatedItem : NegotiatedItems) {
    const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
        [NegotiatedItem](const FEconItem& EconItem) { return EconItem.ItemID == NegotiatedItem->ItemID; });
    check(EconItem);

    BoughtAtPrice += Item->PriceData.BoughtAt;
    MarketPrice += EconItem->CurrentPrice;
    OfferedPrice += MarketPrice;
  }
}

FNextDialogueRes UNegotiationSystem::NPCNegotiationTurn() {
  if (CustomerOfferResponse.Accepted) AcceptOffer();
  else if (CustomerOfferResponse.CounterOffer > 0) OfferPrice(CustomerOfferResponse.CounterOffer);
  else RejectOffer();

  return DialogueSystem->StartDialogue(CustomerOfferResponse.ResponseDialogue);
}

// todo-low: check player has enough money for all negotiation funcs.
void UNegotiationSystem::OfferPrice(float Price) {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::OfferPrice);

  if (NegotiationState == ENegotiationState::NpcConsider)
    CustomerOfferResponse = CustomerAI->NegotiationAI->ConsiderOffer(Type == NegotiationType::PlayerSell ? true : false,
                                                                     MarketPrice, OfferedPrice, Price);

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

    for (const UItemBase* NegotiatedItem : NegotiatedItems) Store->ItemSold(NegotiatedItem, OfferedPrice);
  } else {
    check(PlayerInventory);

    for (UItemBase* NegotiatedItem : NegotiatedItems) Store->ItemBought(NegotiatedItem, OfferedPrice);
    for (const UItemBase* NegotiatedItem : NegotiatedItems) PlayerInventory->AddItem(NegotiatedItem);
  }

  CustomerAI->PostNegotiation();
  if (bIsQuestAssociated) QuestManager->CompleteQuestChain(QuestComponent, {}, true);

  NegotiatedItems.Empty();
  CustomerAI = nullptr;
  bIsQuestAssociated = false;
  QuestComponent = nullptr;
  FromInventory = nullptr;
}

void UNegotiationSystem::NegotiationFailure() {
  CustomerAI->PostNegotiation();
  if (bIsQuestAssociated) QuestManager->CompleteQuestChain(QuestComponent, {}, false);

  NegotiatedItems.Empty();
  CustomerAI = nullptr;
  bIsQuestAssociated = false;
  QuestComponent = nullptr;
  FromInventory = nullptr;
}