#include "NegotiationSystem.h"
#include "CoreFwd.h"
#include "GameplayTagContainer.h"
#include "Logging/LogVerbosity.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Quest/QuestManager.h"
#include "store_playground/Quest/QuestComponent.h"
#include "store_playground/Steam/SteamManager.h"

UNegotiationSystem::UNegotiationSystem() {
  NStateTransitions = {
      // FNStateAction{ENegotiationState::None, ENegotiationAction::OfferPrice, ENegotiationState::PlayerConsider},
      FNStateAction{ENegotiationState::None, ENegotiationAction::NpcRequest, ENegotiationState::NpcRequest},
      FNStateAction{ENegotiationState::None, ENegotiationAction::NpcStockCheckRequest,
                    ENegotiationState::NpcStockCheckRequest},

      FNStateAction{ENegotiationState::NpcRequest, ENegotiationAction::PlayerReadRequest,
                    ENegotiationState::PlayerConsider},
      FNStateAction{ENegotiationState::NpcRequest, ENegotiationAction::Reject, ENegotiationState::Rejected},

      // * Alternatively, stock check.
      FNStateAction{ENegotiationState::NpcStockCheckRequest, ENegotiationAction::PlayerReadRequest,
                    ENegotiationState::PlayerStockCheck},
      FNStateAction{ENegotiationState::NpcStockCheckRequest, ENegotiationAction::Reject, ENegotiationState::Rejected},

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

auto UNegotiationSystem::AddVarsToDialogueText(const FText& DialogueText) -> FText {
  FString NewDialogue = DialogueText.ToString();
  NewDialogue = NewDialogue.Replace(
      *FString::Printf(TEXT("{%s}"), *UEnum::GetDisplayValueAsText(NegDialogueVars::WantedItemType).ToString()),
      *WantedItemType.WantedItemTypeName.ToString());
  NewDialogue =
      NewDialogue.Replace(*FString::Printf(TEXT("{%s}"), *UEnum::GetValueAsString(NegDialogueVars::ItemPrice)),
                          *FString::Printf(TEXT("%.0f¬"), MarketPrice));

  return FText::FromString(*NewDialogue);
}

void UNegotiationSystem::StartNegotiation(UCustomerAIComponent* _CustomerAI,
                                          UItemBase* Item,
                                          UInventoryComponent* _FromInventory,
                                          bool _bIsQuestAssociated,
                                          UQuestComponent* _QuestComponent) {
  check(_CustomerAI && _CustomerAI);

  NegotiationState = ENegotiationState::None;
  NegotiatedItems.Empty();

  Type = _CustomerAI->NegotiationAIDetails.RequestType == ECustomerRequestType::SellItem ? NegotiationType::PlayerBuy
                                                                                         : NegotiationType::PlayerSell;
  CustomerAI = _CustomerAI;
  bIsQuestAssociated = _bIsQuestAssociated;
  QuestComponent = _QuestComponent;

  BoughtAtPrice = 0;
  MarketPrice = 0;
  OfferedPrice = 0;
  CustomerOfferResponse = {false, 0, {}};

  switch (CustomerAI->NegotiationAIDetails.RequestType) {
    case ECustomerRequestType::StockCheck: WantedItemType = CustomerAI->NegotiationAIDetails.WantedItemType; break;
    case ECustomerRequestType::BuyStockItem:
    case ECustomerRequestType::SellItem: {
      NegotiatedItems.Add(Item);
      FromInventory = _FromInventory;

      for (const UItemBase* NegotiatedItem : NegotiatedItems) {
        const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
            [NegotiatedItem](const FEconItem& EconItem) { return EconItem.ItemID == NegotiatedItem->ItemID; });
        check(EconItem);

        BoughtAtPrice += CustomerAI->NegotiationAIDetails.RequestType != ECustomerRequestType::SellItem
                             ? Item->PlayerPriceData.BoughtAt
                             : 0;
        MarketPrice += EconItem->CurrentPrice;
        OfferedPrice += MarketPrice;
      }
      break;
    }
    default: checkNoEntry(); break;
  }

  DialogueSystem->ResetDialogue();
}

// ? Turn FNextDialogueRes into using the dialogue system directly in the ui?
FNextDialogueRes UNegotiationSystem::NPCRequestNegotiation() {
  if (CustomerAI->NegotiationAIDetails.RequestType == ECustomerRequestType::StockCheck) {
    NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::NpcStockCheckRequest);

    TArray<FDialogueData> StockCheckDialogues =
        CustomerAI->NegotiationAIDetails.DialoguesMap[ENegotiationDialogueType::StockCheckRequest].Dialogues;
    for (FDialogueData& Dialogue : StockCheckDialogues)
      Dialogue.DialogueText = AddVarsToDialogueText(Dialogue.DialogueText);

    return DialogueSystem->StartDialogue(StockCheckDialogues, CustomerAI->CustomerName.ToString());
  }

  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::NpcRequest);
  return Type == NegotiationType::PlayerSell
             ? DialogueSystem->StartDialogue(
                   CustomerAI->NegotiationAIDetails.DialoguesMap[ENegotiationDialogueType::BuyItemRequest].Dialogues,
                   CustomerAI->CustomerName.ToString())
             : DialogueSystem->StartDialogue(
                   CustomerAI->NegotiationAIDetails.DialoguesMap[ENegotiationDialogueType::SellItemRequest].Dialogues,
                   CustomerAI->CustomerName.ToString());
}

void UNegotiationSystem::PlayerReadRequest() {
  CustomerAI->StartNegotiation();

  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::PlayerReadRequest);
}
void UNegotiationSystem::PlayerLeaveRequest() { CustomerAI->LeaveRequestDialogue(); }

// todo-low: Support multiple items.
void UNegotiationSystem::PlayerShowItem(UItemBase* Item, UInventoryComponent* _FromInventory) {
  check(Item && _FromInventory);
  // * FromInventory can be player inventory or stock display inventory.

  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::PlayerShowItem);

  CustomerOfferResponse = CustomerAIManager->ConsiderStockCheck(CustomerAI, Item);
  if (!CustomerOfferResponse.Accepted) return;

  NegotiatedItems.Empty();
  NegotiatedItems.Add(Item);
  FromInventory = _FromInventory;

  for (const UItemBase* NegotiatedItem : NegotiatedItems) {
    const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
        [NegotiatedItem](const FEconItem& EconItem) { return EconItem.ItemID == NegotiatedItem->ItemID; });
    check(EconItem);

    BoughtAtPrice += Item->PlayerPriceData.BoughtAt;
    MarketPrice += EconItem->CurrentPrice;
    OfferedPrice += MarketPrice;
  }

  SteamManager->AchBehaviour(FGameplayTag::RequestGameplayTag("AchBehaviour.NegShowCorrectItem"));
}

FNextDialogueRes UNegotiationSystem::NPCNegotiationTurn() {
  if (CustomerOfferResponse.Accepted) AcceptOffer();
  else if (CustomerOfferResponse.CounterOffer > 0) OfferPrice(CustomerOfferResponse.CounterOffer);
  else RejectOffer();

  return DialogueSystem->StartDialogue(CustomerOfferResponse.ResponseDialogue,
                                       CustomerOfferResponse.CustomerName.ToString());
}

// todo-low: check player has enough money for all negotiation funcs.
void UNegotiationSystem::OfferPrice(float Price) {
  NegotiationState = GetNextNegotiationState(NegotiationState, ENegotiationAction::OfferPrice);

  if (NegotiationState == ENegotiationState::NpcConsider)
    CustomerOfferResponse = CustomerAIManager->ConsiderOffer(CustomerAI, NegotiatedItems[0], OfferedPrice, Price);

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

void UNegotiationSystem::NegotiationSuccess() {
  if (Type == NegotiationType::PlayerSell) {
    check(FromInventory);

    for (const UItemBase* NegotiatedItem : NegotiatedItems) Store->NegItemSold(NegotiatedItem, OfferedPrice);
    if (FromInventory == PlayerInventory)
      for (const UItemBase* NegotiatedItem : NegotiatedItems) PlayerInventory->RemoveItem(NegotiatedItem);
    else
      for (const UItemBase* NegotiatedItem : NegotiatedItems) Store->NegStockItemSold(NegotiatedItem);

    SteamManager->AchBehaviour(FGameplayTag::RequestGameplayTag("AchBehaviour.NegSellItem"));
  } else if (Type == NegotiationType::PlayerBuy) {
    check(PlayerInventory);

    for (UItemBase* NegotiatedItem : NegotiatedItems) Store->ItemBought(NegotiatedItem, OfferedPrice);
    for (const UItemBase* NegotiatedItem : NegotiatedItems) {
      // Checks are done in the negotiation process so this should always be true.
      check(CanTransferItem(PlayerInventory, NegotiatedItem));

      PlayerInventory->AddItem(NegotiatedItem);
    }
  }

  if (bIsQuestAssociated) QuestManager->CompleteQuestChain(QuestComponent, {}, true);
}

void UNegotiationSystem::NegotiationFailure() {
  if (bIsQuestAssociated) QuestManager->CompleteQuestChain(QuestComponent, {}, false);
}

void UNegotiationSystem::NegotiationComplete() {
  CustomerAI->PostNegotiation();

  NegotiationState = ENegotiationState::None;

  NegotiatedItems.Empty();
  CustomerAI = nullptr;
  bIsQuestAssociated = false;
  QuestComponent = nullptr;
  FromInventory = nullptr;
}