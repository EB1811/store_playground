#include "NegotiationSystem.h"

std::map<ENegotiationState, FText> NegotiationStateToName = {
    {ENegotiationState::None, FText::FromString(TEXT("None"))},
    {ENegotiationState::NPCRequest, FText::FromString(TEXT("NPC Request"))},
    {ENegotiationState::PlayerOffer, FText::FromString(TEXT("Player Offer"))},
    {ENegotiationState::NPCConsider, FText::FromString(TEXT("NPC Consider"))},
    {ENegotiationState::Accepted, FText::FromString(TEXT("Accepted"))},
    {ENegotiationState::Rejected, FText::FromString(TEXT("Rejected"))},
};

struct State {
  ENegotiationState initial;
  ENegotiationAction action;
  ENegotiationState next;
};
std::array<State, 8> StateTransitions = {
    State{ENegotiationState::None, ENegotiationAction::OfferPrice, ENegotiationState::PlayerOffer},
    State{ENegotiationState::None, ENegotiationAction::NPCRequest, ENegotiationState::NPCRequest},

    State{ENegotiationState::NPCRequest, ENegotiationAction::Consider, ENegotiationState::PlayerOffer},

    State{ENegotiationState::PlayerOffer, ENegotiationAction::OfferPrice, ENegotiationState::NPCConsider},
    State{ENegotiationState::PlayerOffer, ENegotiationAction::Accept, ENegotiationState::Accepted},

    State{ENegotiationState::NPCConsider, ENegotiationAction::Accept, ENegotiationState::Accepted},
    State{ENegotiationState::NPCConsider, ENegotiationAction::Reject, ENegotiationState::Rejected},
    State{ENegotiationState::NPCConsider, ENegotiationAction::OfferPrice, ENegotiationState::PlayerOffer},
};

ENegotiationState GetNextNegotiationState(ENegotiationState State, ENegotiationAction Action) {
  for (const struct State& Transition : StateTransitions)
    if (Transition.initial == State && Transition.action == Action) return Transition.next;

  return ENegotiationState::None;
}