#include "NegotiationWidget.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Button.h"

void UNegotiationWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  OfferButton->OnClicked.AddDynamic(this, &UNegotiationWidget::OnOfferButtonClicked);
  AcceptButton->OnClicked.AddDynamic(this, &UNegotiationWidget::OnAcceptButtonClicked);
  RejectButton->OnClicked.AddDynamic(this, &UNegotiationWidget::OnRejectButtonClicked);
}

void UNegotiationWidget::RefreshNegotiationWhole() {
  ItemName->SetText(NegotiationRef->NegotiatedItem->FlavorData.TextData.Name);
  BasePrice->SetText(FText::FromString(FString::FromInt(NegotiationRef->BasePrice)));
  ItemIcon->SetBrushFromTexture(NegotiationRef->NegotiatedItem->AssetData.Icon);
  if (NegotiationRef->Quantity > 1)
    ItemQuantity->SetText(FText::FromString(FString::FromInt(NegotiationRef->Quantity)));
  else
    ItemQuantity->SetText(FText::FromString(""));
  NegotiationStateText->SetText(NegotiationStateToName[NegotiationRef->NegotiationState]);
  OfferedPrice->SetText(FText::FromString(
      FString::FromInt((NegotiationRef->OfferedPrice > 0 ? NegotiationRef->OfferedPrice : NegotiationRef->BasePrice))));

  PlayerOfferedPrice->SetValue(NegotiationRef->OfferedPrice > 0 ? NegotiationRef->OfferedPrice
                                                                : NegotiationRef->BasePrice);
  PlayerOfferedPrice->SetMaxValue(NegotiationRef->BasePrice * 2);
  PlayerOfferedPrice->SetMinValue(NegotiationRef->BasePrice / 2);
}

void UNegotiationWidget::RefreshNegotiationState() {
  OfferedPrice->SetText(FText::FromString(
      FString::FromInt(NegotiationRef->OfferedPrice > 0 ? NegotiationRef->OfferedPrice : NegotiationRef->BasePrice)));
  NegotiationStateText->SetText(NegotiationStateToName[NegotiationRef->NegotiationState]);

  PlayerOfferedPrice->SetValue(NegotiationRef->OfferedPrice > 0 ? NegotiationRef->OfferedPrice
                                                                : NegotiationRef->BasePrice);
}

// ? Could return a response with updated data for UI.
void UNegotiationWidget::OnOfferButtonClicked() {
  NegotiationRef->OfferPrice(Negotiator::Player, PlayerOfferedPrice->GetValue());
  RefreshNegotiationState();
}

void UNegotiationWidget::OnAcceptButtonClicked() {
  NegotiationRef->AcceptOffer(Negotiator::Player);
  RefreshNegotiationState();

  if (CloseNegotiationUI != nullptr) CloseNegotiationUI();
}

void UNegotiationWidget::OnRejectButtonClicked() {
  NegotiationRef->RejectOffer(Negotiator::Player);
  RefreshNegotiationState();

  if (CloseNegotiationUI != nullptr) CloseNegotiationUI();
}