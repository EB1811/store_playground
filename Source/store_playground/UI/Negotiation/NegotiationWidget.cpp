#include "NegotiationWidget.h"
#include "NegotiationElementWidget.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/AI/CustomerDataStructs.h"
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

  DialogueWidget->CloseDialogueUI = [this] { OnReadDialogueButtonClicked(); };
  StockCheckWidget->ShowItemFunc = [this](UItemBase* Item, UInventoryComponent* FromInventory) {
    ShowItem(Item, FromInventory);
  };

  RefreshNegotiationState();
}

void UNegotiationWidget::StartNegotiation() {
  check(PlayerInventoryRef && NegotiationSystemRef);
  check(NegotiationSystemRef->NegotiatedItems.Num() > 0);

  auto FirstItem = NegotiationSystemRef->NegotiatedItems[0];
  NegotiationElementWidget->ItemName->SetText(FirstItem->TextData.Name);
  NegotiationElementWidget->BoughtAtPrice->SetText(
      FText::FromString(FString::FromInt(NegotiationSystemRef->BoughtAtPrice)));
  NegotiationElementWidget->MarketPrice->SetText(
      FText::FromString(FString::FromInt(NegotiationSystemRef->MarketPrice)));
  NegotiationElementWidget->ItemIcon->SetBrushFromTexture(FirstItem->AssetData.Icon);

  NegotiationElementWidget->PlayerOfferedPrice->SetMaxValue(NegotiationSystemRef->BoughtAtPrice * 2);
  NegotiationElementWidget->PlayerOfferedPrice->SetMinValue(NegotiationSystemRef->BoughtAtPrice / 2);
}

void UNegotiationWidget::RefreshNegotiationState() {
  NegotiationElementWidget->OfferedPrice->SetText(
      FText::FromString(FString::FromInt(NegotiationSystemRef->OfferedPrice)));
  NegotiationElementWidget->NegotiationStateText->SetText(
      UEnum::GetDisplayValueAsText(NegotiationSystemRef->NegotiationState));

  NegotiationElementWidget->PlayerOfferedPrice->SetValue(NegotiationSystemRef->OfferedPrice);

  switch (NegotiationSystemRef->NegotiationState) {
    case ENegotiationState::None: {
      auto Dialogue = NegotiationSystemRef->NPCRequestNegotiation();

      // DialogueWidget->InitDialogueUI(NegotiationSystemRef->DialogueSystem);
      DialogueWidget->SetVisibility(ESlateVisibility::Visible);

      StockCheckWidget->SetVisibility(ESlateVisibility::Hidden);
      NegotiationElementWidget->SetVisibility(ESlateVisibility::Hidden);
      break;
    }
    case ENegotiationState::PlayerStockCheck: {
      StockCheckWidget->SetVisibility(ESlateVisibility::Visible);

      DialogueWidget->SetVisibility(ESlateVisibility::Hidden);
      NegotiationElementWidget->SetVisibility(ESlateVisibility::Hidden);
      break;
    }
    case ENegotiationState::NpcStockCheckConsider: {
      auto Dialogue = NegotiationSystemRef->NPCNegotiationTurn();

      // DialogueWidget->InitDialogueUI(NegotiationSystemRef->DialogueSystem);
      DialogueWidget->SetVisibility(ESlateVisibility::Visible);

      StockCheckWidget->SetVisibility(ESlateVisibility::Hidden);
      NegotiationElementWidget->SetVisibility(ESlateVisibility::Hidden);

      break;
    }
    case ENegotiationState::PlayerConsider: {
      NegotiationElementWidget->SetVisibility(ESlateVisibility::Visible);

      DialogueWidget->SetVisibility(ESlateVisibility::Hidden);
      StockCheckWidget->SetVisibility(ESlateVisibility::Hidden);
      break;
    }

    case ENegotiationState::NpcConsider: {
      auto Dialogue = NegotiationSystemRef->NPCNegotiationTurn();

      // DialogueWidget->InitDialogueUI(NegotiationSystemRef->DialogueSystem);
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
    case ENegotiationState::NpcRequest: {
      NegotiationSystemRef->PlayerReadRequest();
      StartNegotiation();
      break;
    }
    case ENegotiationState::NpcStockCheckRequest: {
      NegotiationSystemRef->PlayerReadRequest();
      StockCheckWidget->InitStockCheckUI(PlayerInventoryRef, NegotiationSystemRef->WantedItemType.WantedItemTypeName);
      break;
    }
    case ENegotiationState::PlayerConsider: {
      StartNegotiation();
      break;
    }
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