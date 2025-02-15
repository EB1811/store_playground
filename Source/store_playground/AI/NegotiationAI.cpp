#include "NegotiationAI.h"

std::map<ENegotiatorAttitude, float> AttitudeToPercentAcceptance = {
    {ENegotiatorAttitude::FRIENDLY, 1.15},
    {ENegotiatorAttitude::NEUTRAL, 1.1},
    {ENegotiatorAttitude::HOSTILE, 1.05},
};

OfferResponse UNegotiationAI::ConsiderOffer(float BasePrice, float OfferedPrice) const {
  float OfferedPercent = OfferedPrice / BasePrice;
  if (OfferedPercent <= AttitudeToPercentAcceptance[Attitude]) return {true, 0};

  float adjustedPercent = AttitudeToPercentAcceptance[Attitude] - FMath::FRandRange(0.01f, 0.2f);
  return {false, BasePrice * adjustedPercent};
}