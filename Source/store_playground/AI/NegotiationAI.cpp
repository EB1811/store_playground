#include "NegotiationAI.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"

FOfferResponse UNegotiationAI::ConsiderOffer(bool NpcBuying,
                                             float BasePrice,
                                             float LastOfferedPrice,
                                             float PlayerOfferedPrice) const {
  float OfferedPercent = PlayerOfferedPrice / BasePrice;
  if (NpcBuying && OfferedPercent <= AcceptancePercentage)
    return {0, true, DialoguesMap[ENegotiationDialogueType::Accept].Dialogues};
  if (!NpcBuying && OfferedPercent >= AcceptancePercentage)
    return {0, true, DialoguesMap[ENegotiationDialogueType::Accept].Dialogues};

  float adjustedPercent = AcceptancePercentage - FMath::FRandRange(0.01f, 0.1f);

  return {LastOfferedPrice * adjustedPercent, false,
          OfferedPercent > 1.4f ? DialoguesMap[ENegotiationDialogueType::ConsiderTooHigh].Dialogues
                                : DialoguesMap[ENegotiationDialogueType::ConsiderClose].Dialogues};
}

FOfferResponse UNegotiationAI::ConsiderStockCheck(UItemBase* Item) const {
  if (Item->ItemID == RelevantItem->ItemID)
    return {0, true, DialoguesMap[ENegotiationDialogueType::StockCheckAccept].Dialogues};

  return {0, false, DialoguesMap[ENegotiationDialogueType::StockCheckReject].Dialogues};
}