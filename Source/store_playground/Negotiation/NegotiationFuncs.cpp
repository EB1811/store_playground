#include "NegotiationSystem.h"

std::map<ENegotiationState, FText> NegotiationStateToName = {
    {ENegotiationState::None, FText::FromString(TEXT("None"))},
    {ENegotiationState::NPCTurn, FText::FromString(TEXT("NPC's Turn"))},
    {ENegotiationState::PlayerTurn, FText::FromString(TEXT("Player's Turn"))},
    {ENegotiationState::Accepted, FText::FromString(TEXT("Accepted"))},
    {ENegotiationState::Rejected, FText::FromString(TEXT("Rejected"))},
};

ENegotiationState GetNextNegotiationState(ENegotiationState State, ENegotiationAction Action) {
  switch (Action) {
    case ENegotiationAction::Offer:
      if (State == ENegotiationState::PlayerTurn) return ENegotiationState::NPCTurn;
      if (State == ENegotiationState::NPCTurn) return ENegotiationState::PlayerTurn;
    case ENegotiationAction::Accept: return ENegotiationState::Accepted;
    case ENegotiationAction::Reject: return ENegotiationState::Rejected;
  }

  return ENegotiationState::None;
}