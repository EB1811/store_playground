#include "NegotiationSystem.h"

std::map<ENegotiationState, FText> NegotiationStateToName = {
    {ENegotiationState::NONE, FText::FromString(TEXT("None"))},
    {ENegotiationState::NPC_TURN, FText::FromString(TEXT("NPC's Turn"))},
    {ENegotiationState::PLAYER_TURN, FText::FromString(TEXT("Player's Turn"))},
    {ENegotiationState::ACCEPTED, FText::FromString(TEXT("Accepted"))},
    {ENegotiationState::REJECTED, FText::FromString(TEXT("Rejected"))},
};

ENegotiationState GetNextNegotiationState(ENegotiationState State, ENegotiationAction Action) {
  switch (Action) {
    case ENegotiationAction::OFFER:
      if (State == ENegotiationState::PLAYER_TURN) return ENegotiationState::NPC_TURN;
      if (State == ENegotiationState::NPC_TURN) return ENegotiationState::PLAYER_TURN;
    case ENegotiationAction::ACCEPT: return ENegotiationState::ACCEPTED;
    case ENegotiationAction::REJECT: return ENegotiationState::REJECTED;
  }

  return ENegotiationState::NONE;
}