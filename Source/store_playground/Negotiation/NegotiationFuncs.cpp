#include "Misc/AssertionMacros.h"
#include "NegotiationSystem.h"

std::map<ENegotiationState, FText> NegotiationStateToName = {
    {ENegotiationState::None, FText::FromString(TEXT("None"))},
    {ENegotiationState::NPCRequest, FText::FromString(TEXT("NPC Request"))},
    {ENegotiationState::PlayerConsider, FText::FromString(TEXT("Player Offer"))},
    {ENegotiationState::NPCConsider, FText::FromString(TEXT("NPC Consider"))},
    {ENegotiationState::Accepted, FText::FromString(TEXT("Accepted"))},
    {ENegotiationState::Rejected, FText::FromString(TEXT("Rejected"))},
};

struct State {
  ENegotiationState initial;
  ENegotiationAction action;
  ENegotiationState next;
};
std::array<State, 9> StateTransitions = {
    State{ENegotiationState::None, ENegotiationAction::OfferPrice, ENegotiationState::PlayerConsider},
    State{ENegotiationState::None, ENegotiationAction::NPCRequest, ENegotiationState::NPCRequest},

    State{ENegotiationState::NPCRequest, ENegotiationAction::PlayerReadRequest, ENegotiationState::PlayerConsider},

    State{ENegotiationState::PlayerConsider, ENegotiationAction::OfferPrice, ENegotiationState::NPCConsider},
    State{ENegotiationState::PlayerConsider, ENegotiationAction::Accept, ENegotiationState::Accepted},
    State{ENegotiationState::PlayerConsider, ENegotiationAction::Reject, ENegotiationState::Rejected},

    State{ENegotiationState::NPCConsider, ENegotiationAction::OfferPrice, ENegotiationState::PlayerConsider},
    State{ENegotiationState::NPCConsider, ENegotiationAction::Accept, ENegotiationState::Accepted},
    State{ENegotiationState::NPCConsider, ENegotiationAction::Reject, ENegotiationState::Rejected},
};

ENegotiationState GetNextNegotiationState(ENegotiationState State, ENegotiationAction Action) {
  for (const struct State& Transition : StateTransitions)
    if (Transition.initial == State && Transition.action == Action) return Transition.next;

  // temp Crashing for debugging purposes.
  checkf(false, TEXT("Invalid Action %d for State %d"), (int)Action, (int)State);

  return ENegotiationState::None;
}