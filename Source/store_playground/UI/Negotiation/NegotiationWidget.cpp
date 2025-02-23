#include "NegotiationWidget.h"
#include "NegotiationElementWidget.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/AI/NegotiationAI.h"
#include "store_playground/UI/Dialogue/DialogueWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Button.h"

void UNegotiationWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  // Temp: Directly setting values.
  NegotiationElementWidget->OfferButton->OnClicked.AddDynamic(this, &UNegotiationWidget::OnOfferButtonClicked);
  NegotiationElementWidget->AcceptButton->OnClicked.AddDynamic(this, &UNegotiationWidget::OnAcceptButtonClicked);
  NegotiationElementWidget->RejectButton->OnClicked.AddDynamic(this, &UNegotiationWidget::OnRejectButtonClicked);
}

void UNegotiationWidget::InitNegotiationUI() {
  NegotiationElementWidget->ItemName->SetText(NegotiationSystemRef->NegotiatedItem->FlavorData.TextData.Name);
  NegotiationElementWidget->BasePrice->SetText(FText::FromString(FString::FromInt(NegotiationSystemRef->BasePrice)));
  NegotiationElementWidget->ItemIcon->SetBrushFromTexture(NegotiationSystemRef->NegotiatedItem->AssetData.Icon);
  if (NegotiationSystemRef->Quantity > 1)
    NegotiationElementWidget->ItemQuantity->SetText(
        FText::FromString(FString::FromInt(NegotiationSystemRef->Quantity)));
  else
    NegotiationElementWidget->ItemQuantity->SetText(FText::FromString(""));

  NegotiationElementWidget->PlayerOfferedPrice->SetMaxValue(NegotiationSystemRef->BasePrice * 2);
  NegotiationElementWidget->PlayerOfferedPrice->SetMinValue(NegotiationSystemRef->BasePrice / 2);

  DialogueWidget->CloseDialogueUI = [this] { OnReadDialogueButtonClicked(); };

  RefreshNegotiationState();
}

void UNegotiationWidget::RefreshNegotiationState() {
  NegotiationElementWidget->OfferedPrice->SetText(FText::FromString(FString::FromInt(
      NegotiationSystemRef->OfferedPrice > 0 ? NegotiationSystemRef->OfferedPrice : NegotiationSystemRef->BasePrice)));
  NegotiationElementWidget->NegotiationStateText->SetText(
      NegotiationStateToName[NegotiationSystemRef->NegotiationState]);

  NegotiationElementWidget->PlayerOfferedPrice->SetValue(
      NegotiationSystemRef->OfferedPrice > 0 ? NegotiationSystemRef->OfferedPrice : NegotiationSystemRef->BasePrice);

  UE_LOG(LogTemp, Warning, TEXT("RefreshNegotiationState: %d"), NegotiationSystemRef->NegotiationState);
  switch (NegotiationSystemRef->NegotiationState) {
    case ENegotiationState::None: {
      auto Dialogue = NegotiationSystemRef->NPCRequestNegotiation();

      DialogueWidget->InitDialogueUI(NegotiationSystemRef->DialogueSystem);
      DialogueWidget->SetVisibility(ESlateVisibility::Visible);

      NegotiationElementWidget->SetVisibility(ESlateVisibility::Hidden);
      break;
    }
    case ENegotiationState::NPCConsider: {
      auto Dialogue = NegotiationSystemRef->NPCNegotiationTurn();

      DialogueWidget->InitDialogueUI(NegotiationSystemRef->DialogueSystem);
      DialogueWidget->SetVisibility(ESlateVisibility::Visible);

      NegotiationElementWidget->SetVisibility(ESlateVisibility::Hidden);

      break;
    }
    case ENegotiationState::PlayerConsider: {
      DialogueWidget->SetVisibility(ESlateVisibility::Hidden);
      NegotiationElementWidget->SetVisibility(ESlateVisibility::Visible);
      break;
    }
    case ENegotiationState::Accepted:
    case ENegotiationState::Rejected:
      check(RefreshInventoryUI && CloseNegotiationUI);
      RefreshInventoryUI();
      CloseNegotiationUI();
      break;
    default: check(false); break;
  }
}

void UNegotiationWidget::OnReadDialogueButtonClicked() {
  switch (NegotiationSystemRef->NegotiationState) {
    case ENegotiationState::NPCRequest: NegotiationSystemRef->PlayerReadRequest(); break;
    default: break;
  }

  RefreshNegotiationState();
}

// ? Could return a response with updated data for UI.
void UNegotiationWidget::OnOfferButtonClicked() {
  NegotiationSystemRef->OfferPrice(Negotiator::Player, NegotiationElementWidget->PlayerOfferedPrice->GetValue());
  RefreshNegotiationState();
}

void UNegotiationWidget::OnAcceptButtonClicked() {
  NegotiationSystemRef->AcceptOffer(Negotiator::Player);
  RefreshNegotiationState();
}

void UNegotiationWidget::OnRejectButtonClicked() {
  NegotiationSystemRef->RejectOffer(Negotiator::Player);
  RefreshNegotiationState();

  CloseNegotiationUI();
}