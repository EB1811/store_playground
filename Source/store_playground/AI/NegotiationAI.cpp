#include "NegotiationAI.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"

FOfferResponse UNegotiationAI::ConsiderOffer(bool NpcBuying,
                                             float MarketPrice,
                                             float LastOfferedPrice,
                                             float PlayerOfferedPrice) const {
  if (FMath::IsNearlyEqual(PlayerOfferedPrice, LastOfferedPrice, KINDA_SMALL_NUMBER))
    return {true, 0, DialoguesMap[ENegotiationDialogueType::Accept].Dialogues, CustomerName};

  float OfferedPercent = PlayerOfferedPrice / MarketPrice;
  if (NpcBuying && OfferedPercent <= 1.0 + AcceptancePercentage + 0.01f)
    return {true, 0, DialoguesMap[ENegotiationDialogueType::Accept].Dialogues, CustomerName};
  if (!NpcBuying && OfferedPercent >= 1.0 - AcceptancePercentage - 0.01f)
    return {true, 0, DialoguesMap[ENegotiationDialogueType::Accept].Dialogues, CustomerName};

  // Leave negotiation if the difference is too high.
  float AcceptanceDiff =
      NpcBuying ? OfferedPercent - 1.0 + AcceptancePercentage : 1.0 - AcceptancePercentage - OfferedPercent;
  if (AcceptanceDiff > 0.33f)
    return {false, 0,
            NpcBuying ? DialoguesMap[ENegotiationDialogueType::BuyItemTooHigh].Dialogues
                      : DialoguesMap[ENegotiationDialogueType::SellItemTooLow].Dialogues,
            CustomerName};

  float adjustedPercent =
      NpcBuying
          ? FMath::Min((LastOfferedPrice / MarketPrice) + FMath::FRandRange(0.01f, 0.1f), 1.0 + AcceptancePercentage)
          : FMath::Max((LastOfferedPrice / MarketPrice) - FMath::FRandRange(0.01f, 0.1f), 1.0 - AcceptancePercentage);
  UE_LOG(LogTemp, Warning,
         TEXT("ConsiderOffer: NpcBuying: %d, MarketPrice: %f, PlayerOfferedPrice: %f, OfferedPercent: %f, "
              "AcceptancePercentage: %f, LastOfferedPrice: %f, AdjustedPercent: %f"),
         NpcBuying, MarketPrice, PlayerOfferedPrice, OfferedPercent, AcceptancePercentage, LastOfferedPrice,
         adjustedPercent);
  return {false, MarketPrice * adjustedPercent,
          AcceptanceDiff > 0.2 ? (NpcBuying ? DialoguesMap[ENegotiationDialogueType::BuyItemTooHigh].Dialogues
                                            : DialoguesMap[ENegotiationDialogueType::SellItemTooLow].Dialogues)
                               : (NpcBuying ? DialoguesMap[ENegotiationDialogueType::BuyItemClose].Dialogues
                                            : DialoguesMap[ENegotiationDialogueType::SellItemClose].Dialogues),
          CustomerName};
}

FOfferResponse UNegotiationAI::ConsiderStockCheck(const UItemBase* Item) const {
  if (Item->ItemType == WantedItemType.ItemType && Item->ItemEconType == WantedItemType.ItemEconType)
    return {true, 0, DialoguesMap[ENegotiationDialogueType::StockCheckAccept].Dialogues, CustomerName};

  return {false, 0, DialoguesMap[ENegotiationDialogueType::StockCheckReject].Dialogues, CustomerName};
}