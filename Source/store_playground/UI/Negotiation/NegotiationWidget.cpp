#include "NegotiationWidget.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/UI/Dialogue/DialogueWidget.h"
#include "store_playground/UI/Dialogue/DialogueBoxWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Button.h"

void UNegotiationWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  OfferButton->OnClicked.AddDynamic(this, &UNegotiationWidget::OnOfferButtonClicked);
  AcceptButton->OnClicked.AddDynamic(this, &UNegotiationWidget::OnAcceptButtonClicked);
  RejectButton->OnClicked.AddDynamic(this, &UNegotiationWidget::OnRejectButtonClicked);
  DialogueBoxWidget->NextButton->OnClicked.AddDynamic(this, &UNegotiationWidget::OnReadDialogueButtonClicked);
}

void UNegotiationWidget::RefreshNegotiationWhole() {
  ItemName->SetText(NegotiationRef->NegotiatedItem->FlavorData.TextData.Name);
  BasePrice->SetText(FText::FromString(FString::FromInt(NegotiationRef->BasePrice)));
  ItemIcon->SetBrushFromTexture(NegotiationRef->NegotiatedItem->AssetData.Icon);
  if (NegotiationRef->Quantity > 1)
    ItemQuantity->SetText(FText::FromString(FString::FromInt(NegotiationRef->Quantity)));
  else
    ItemQuantity->SetText(FText::FromString(""));

  PlayerOfferedPrice->SetMaxValue(NegotiationRef->BasePrice * 2);
  PlayerOfferedPrice->SetMinValue(NegotiationRef->BasePrice / 2);

  RefreshNegotiationState();
}

void UNegotiationWidget::RefreshNegotiationState() {
  OfferedPrice->SetText(FText::FromString(
      FString::FromInt(NegotiationRef->OfferedPrice > 0 ? NegotiationRef->OfferedPrice : NegotiationRef->BasePrice)));
  NegotiationStateText->SetText(NegotiationStateToName[NegotiationRef->NegotiationState]);

  PlayerOfferedPrice->SetValue(NegotiationRef->OfferedPrice > 0 ? NegotiationRef->OfferedPrice
                                                                : NegotiationRef->BasePrice);

  switch (NegotiationRef->NegotiationState) {
    case ENegotiationState::NPCRequest:
      DialogueBoxWidget->Speaker->SetText(FText::FromString("NPC"));
      DialogueBoxWidget->DialogueText->SetText(FText::FromString("NPC wants Item."));
      DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Next"));
      DialogueBoxWidget->SetVisibility(ESlateVisibility::Visible);
      break;
    case ENegotiationState::NPCConsider:
      DialogueBoxWidget->Speaker->SetText(FText::FromString("NPC"));
      DialogueBoxWidget->DialogueText->SetText(FText::FromString("NPC is considering your offer."));
      DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Next"));
      DialogueBoxWidget->SetVisibility(ESlateVisibility::Visible);
      break;
    case ENegotiationState::PlayerOffer: DialogueBoxWidget->SetVisibility(ESlateVisibility::Hidden); break;
    case ENegotiationState::Accepted:
      DialogueBoxWidget->Speaker->SetText(FText::FromString("NPC"));
      DialogueBoxWidget->DialogueText->SetText(FText::FromString("NPC accepted your offer."));
      DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Close"));
      DialogueBoxWidget->SetVisibility(ESlateVisibility::Visible);
      break;
    default: break;
  }
}

void UNegotiationWidget::OnReadDialogueButtonClicked() {
  switch (NegotiationRef->NegotiationState) {
    case ENegotiationState::NPCRequest: NegotiationRef->Consider(Negotiator::Player); break;
    case ENegotiationState::NPCConsider: NegotiationRef->NPCNegotiationTurn(); break;
    case ENegotiationState::Accepted: return CloseNegotiationUI();
    default: break;
  }

  if (NegotiationRef->NegotiationState == ENegotiationState::Accepted) return CloseNegotiationUI();

  RefreshNegotiationState();
}

// ? Could return a response with updated data for UI.
void UNegotiationWidget::OnOfferButtonClicked() {
  NegotiationRef->OfferPrice(Negotiator::Player, PlayerOfferedPrice->GetValue());
  RefreshNegotiationState();
}

void UNegotiationWidget::OnAcceptButtonClicked() {
  NegotiationRef->AcceptOffer(Negotiator::Player);
  RefreshNegotiationState();

  CloseNegotiationUI();
}

void UNegotiationWidget::OnRejectButtonClicked() {
  NegotiationRef->RejectOffer(Negotiator::Player);
  RefreshNegotiationState();

  CloseNegotiationUI();
}