#include "Misc/AssertionMacros.h"
#include "NegotiationSystem.h"

TMap<ENegotiationState, FText> NegotiationStateToName = {
    {ENegotiationState::None, FText::FromString(TEXT("None"))},
    {ENegotiationState::NpcRequest, FText::FromString(TEXT("NPC Request"))},
    {ENegotiationState::PlayerConsider, FText::FromString(TEXT("Player Offer"))},
    {ENegotiationState::NpcConsider, FText::FromString(TEXT("NPC Consider"))},
    {ENegotiationState::Accepted, FText::FromString(TEXT("Accepted"))},
    {ENegotiationState::Rejected, FText::FromString(TEXT("Rejected"))},
    {ENegotiationState::NpcStockCheckRequest, FText::FromString(TEXT("NPC Request Stock Check"))},
    {ENegotiationState::PlayerStockCheck, FText::FromString(TEXT("Player Stock Check"))},
    {ENegotiationState::NpcStockCheckConsider, FText::FromString(TEXT("NPC Stock Check Response"))},
};

TArray<NStateAction> NStateTransitions = {
    NStateAction{ENegotiationState::None, ENegotiationAction::OfferPrice, ENegotiationState::PlayerConsider},
    NStateAction{ENegotiationState::None, ENegotiationAction::NpcRequest, ENegotiationState::NpcRequest},
    NStateAction{ENegotiationState::None, ENegotiationAction::NpcStockCheckRequest,
                 ENegotiationState::NpcStockCheckRequest},

    NStateAction{ENegotiationState::NpcRequest, ENegotiationAction::PlayerReadRequest,
                 ENegotiationState::PlayerConsider},

    // * Alternatively, stock check.
    NStateAction{ENegotiationState::NpcStockCheckRequest, ENegotiationAction::PlayerReadRequest,
                 ENegotiationState::PlayerStockCheck},
    NStateAction{ENegotiationState::PlayerStockCheck, ENegotiationAction::PlayerShowItem,
                 ENegotiationState::NpcStockCheckConsider},
    NStateAction{ENegotiationState::NpcStockCheckConsider, ENegotiationAction::Accept,
                 ENegotiationState::PlayerConsider},
    NStateAction{ENegotiationState::NpcStockCheckConsider, ENegotiationAction::Reject, ENegotiationState::Rejected},

    // * Negotiation back and forth.
    NStateAction{ENegotiationState::PlayerConsider, ENegotiationAction::OfferPrice, ENegotiationState::NpcConsider},
    NStateAction{ENegotiationState::PlayerConsider, ENegotiationAction::Accept, ENegotiationState::Accepted},
    NStateAction{ENegotiationState::PlayerConsider, ENegotiationAction::Reject, ENegotiationState::Rejected},
    NStateAction{ENegotiationState::NpcConsider, ENegotiationAction::OfferPrice, ENegotiationState::PlayerConsider},
    NStateAction{ENegotiationState::NpcConsider, ENegotiationAction::Accept, ENegotiationState::Accepted},
    NStateAction{ENegotiationState::NpcConsider, ENegotiationAction::Reject, ENegotiationState::Rejected},
};

ENegotiationState GetNextNegotiationState(ENegotiationState NStateAction, ENegotiationAction Action) {
  for (const struct NStateAction& Transition : NStateTransitions)
    if (Transition.initial == NStateAction && Transition.action == Action) return Transition.next;

  // temp Crashing for debugging purposes.
  checkf(false, TEXT("Invalid Action %d for NStateAction %d"), (int)Action, (int)NStateAction);

  return ENegotiationState::None;
}