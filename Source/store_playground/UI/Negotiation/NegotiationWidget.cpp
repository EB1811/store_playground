#include "NegotiationWidget.h"
#include "NegotiationElementWidget.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/AI/NegotiationAI.h"
#include "store_playground/UI/Dialogue/DialogueWidget.h"
#include "store_playground/UI/Negotiation/StockCheckWidget.h"
#include "store_playground/Inventory/InventoryComponent.h"
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
  check(PlayerInventoryRef && NegotiationSystemRef);
  check(NegotiationSystemRef->NegotiatedItems.Num() > 0);

  auto FirstItem = NegotiationSystemRef->NegotiatedItems[0];
  // NegotiationElementWidget->ItemName->SetText(FirstItem->FlavorData.TextData.Name);
  NegotiationElementWidget->BasePrice->SetText(FText::FromString(FString::FromInt(NegotiationSystemRef->BasePrice)));
  NegotiationElementWidget->ItemIcon->SetBrushFromTexture(FirstItem->AssetData.Icon);
  if (NegotiationSystemRef->Quantity > 1)
    NegotiationElementWidget->ItemQuantity->SetText(
        FText::FromString(FString::FromInt(NegotiationSystemRef->Quantity)));
  else
    NegotiationElementWidget->ItemQuantity->SetText(FText::FromString(""));

  NegotiationElementWidget->PlayerOfferedPrice->SetMaxValue(NegotiationSystemRef->BasePrice * 2);
  NegotiationElementWidget->PlayerOfferedPrice->SetMinValue(NegotiationSystemRef->BasePrice / 2);

  DialogueWidget->CloseDialogueUI = [this] { OnReadDialogueButtonClicked(); };

  StockCheckWidget->ShowItemFunc = [this](UItemBase* Item, UInventoryComponent* FromInventory) {
    ShowItem(Item, FromInventory);
  };

  RefreshNegotiationState();
}

void UNegotiationWidget::RefreshNegotiationState() {
  NegotiationElementWidget->OfferedPrice->SetText(FText::FromString(FString::FromInt(
      NegotiationSystemRef->OfferedPrice > 0 ? NegotiationSystemRef->OfferedPrice : NegotiationSystemRef->BasePrice)));
  NegotiationElementWidget->NegotiationStateText->SetText(
      NegotiationStateToName[NegotiationSystemRef->NegotiationState]);

  NegotiationElementWidget->PlayerOfferedPrice->SetValue(
      NegotiationSystemRef->OfferedPrice > 0 ? NegotiationSystemRef->OfferedPrice : NegotiationSystemRef->BasePrice);

  switch (NegotiationSystemRef->NegotiationState) {
    case ENegotiationState::None: {
      auto Dialogue = NegotiationSystemRef->NPCRequestNegotiation();

      DialogueWidget->InitDialogueUI(NegotiationSystemRef->DialogueSystem);
      DialogueWidget->SetVisibility(ESlateVisibility::Visible);

      StockCheckWidget->SetVisibility(ESlateVisibility::Hidden);
      NegotiationElementWidget->SetVisibility(ESlateVisibility::Hidden);
      break;
    }
    case ENegotiationState::PlayerStockCheck: {
      StockCheckWidget->InitStockCheckUI(PlayerInventoryRef, NegotiationSystemRef->NegotiatedItems[0]);

      // TODO: Put this in a function.
      StockCheckWidget->SetVisibility(ESlateVisibility::Visible);
      DialogueWidget->SetVisibility(ESlateVisibility::Hidden);
      NegotiationElementWidget->SetVisibility(ESlateVisibility::Hidden);
      break;
    }
    case ENegotiationState::NpcStockCheckConsider: {
      auto Dialogue = NegotiationSystemRef->NPCNegotiationTurn();

      DialogueWidget->InitDialogueUI(NegotiationSystemRef->DialogueSystem);
      DialogueWidget->SetVisibility(ESlateVisibility::Visible);

      StockCheckWidget->SetVisibility(ESlateVisibility::Hidden);
      NegotiationElementWidget->SetVisibility(ESlateVisibility::Hidden);

      break;
    }
    case ENegotiationState::PlayerConsider: {
      DialogueWidget->SetVisibility(ESlateVisibility::Hidden);
      NegotiationElementWidget->SetVisibility(ESlateVisibility::Visible);
      break;
    }

    case ENegotiationState::NpcConsider: {
      auto Dialogue = NegotiationSystemRef->NPCNegotiationTurn();

      DialogueWidget->InitDialogueUI(NegotiationSystemRef->DialogueSystem);
      DialogueWidget->SetVisibility(ESlateVisibility::Visible);

      StockCheckWidget->SetVisibility(ESlateVisibility::Hidden);
      NegotiationElementWidget->SetVisibility(ESlateVisibility::Hidden);

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
    case ENegotiationState::NpcRequest:
    case ENegotiationState::NpcStockCheckRequest: NegotiationSystemRef->PlayerReadRequest(); break;
    default: break;
  }

  RefreshNegotiationState();
}

void UNegotiationWidget::ShowItem(UItemBase* Item, UInventoryComponent* FromInventory) {
  NegotiationSystemRef->PlayerShowItem(Item, FromInventory);
  RefreshNegotiationState();
}

void UNegotiationWidget::OnOfferButtonClicked() {
  NegotiationSystemRef->OfferPrice(NegotiationElementWidget->PlayerOfferedPrice->GetValue());
  RefreshNegotiationState();
}

void UNegotiationWidget::OnAcceptButtonClicked() {
  NegotiationSystemRef->AcceptOffer();
  RefreshNegotiationState();
}

void UNegotiationWidget::OnRejectButtonClicked() {
  NegotiationSystemRef->RejectOffer();
  RefreshNegotiationState();

  CloseNegotiationUI();
}