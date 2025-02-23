#include "NegotiationAI.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"

std::map<ENegotiatorAttitude, float> AttitudeToPercentAcceptance = {
    {ENegotiatorAttitude::FRIENDLY, 1.15},
    {ENegotiatorAttitude::NEUTRAL, 1.1},
    {ENegotiatorAttitude::HOSTILE, 1.05},
};

FOfferResponse UNegotiationAI::ConsiderOffer(float BasePrice, float LastOfferedPrice, float PlayerOfferedPrice) const {
  float OfferedPercent = PlayerOfferedPrice / BasePrice;
  if (OfferedPercent <= AttitudeToPercentAcceptance[Attitude]) return {0, true};

  float adjustedPercent = AttitudeToPercentAcceptance[Attitude] - FMath::FRandRange(0.01f, 0.1f);
  return {LastOfferedPrice * adjustedPercent, false};
}