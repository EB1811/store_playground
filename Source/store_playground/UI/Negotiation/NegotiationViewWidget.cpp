#include "NegotiationViewWidget.h"
#include "store_playground/UI/Components/RightSlideWidget.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Components/ControlsHelpersWidget.h"
#include "store_playground/UI/Negotiation/PriceSliderWidget.h"
#include "store_playground/UI/Inventory/CompactItemDetailsWidget.h"
#include "store_playground/UI/Negotiation/NegotiationSkillsWidget.h"
#include "store_playground/UI/Negotiation/NegotiationShowItemWidget.h"
#include "store_playground/UI/Negotiation/PriceNegotiationWidget.h"
#include "store_playground/UI/Dialogue/DialogueWidget.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/AI/NegotiationAI.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Logging/LogVerbosity.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/AssertionMacros.h"
#include "Widgets/Notifications/SProgressBar.h"

void UNegotiationViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  OfferAcceptButton->ControlButton->OnClicked.AddDynamic(this, &UNegotiationViewWidget::OfferAcceptClicked);
  RejectButton->ControlButton->OnClicked.AddDynamic(this, &UNegotiationViewWidget::RejectClicked);

  DialogueWidget->SetVisibility(ESlateVisibility::Hidden);
  PriceNegotiationWidget->SetVisibility(ESlateVisibility::Hidden);
  NegotiationShowItemWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UNegotiationViewWidget::ShowItem(UItemBase* Item) {
  NegotiationSystem->PlayerShowItem(Item, PlayerInventoryC);

  if (NegotiationSystem->CustomerOfferResponse.Accepted) PriceNegotiationWidget->InitUI(Store, NegotiationSystem);
  RefreshUI();
}

void UNegotiationViewWidget::OfferAccept() {
  float Price = PriceNegotiationWidget->PriceSliderWidget->PlayerPriceSlider->GetValue();
  check(Price >= 0.0f);

  // if (Price == NegotiationSystem->OfferedPrice) NegotiationSystem->AcceptOffer();
  // else NegotiationSystem->OfferPrice(Price);
  NegotiationSystem->OfferPrice(Price);

  RefreshUI();
}

void UNegotiationViewWidget::Reject() {
  NegotiationSystem->RejectOffer();
  RefreshUI();
}

// Temp
void UNegotiationViewWidget::OfferAcceptClicked() { OfferAccept(); }
void UNegotiationViewWidget::RejectClicked() { Reject(); }

void UNegotiationViewWidget::RefreshUI() {
  OfferAcceptButton->SetVisibility(ESlateVisibility::Hidden);
  RejectButton->SetVisibility(ESlateVisibility::Hidden);

  // * When on npc consider states, advance the state machine.
  // * But first, show the dialogue.
  switch (NegotiationSystem->NegotiationState) {
    case ENegotiationState::None:
    case ENegotiationState::NpcRequest:
    case ENegotiationState::NpcStockCheckRequest: {
      // Handled in InitUI.
      break;
    }
    case ENegotiationState::PlayerStockCheck: {
      NegotiationShowItemWidget->RefreshUI();

      DialogueWidget->SetVisibility(ESlateVisibility::Hidden);
      PriceNegotiationWidget->SetVisibility(ESlateVisibility::Hidden);
      NegotiationShowItemWidget->SetVisibility(ESlateVisibility::Visible);
      break;
    }
    case ENegotiationState::NpcStockCheckConsider: {
      auto Dialogue = NegotiationSystem->NPCNegotiationTurn();
      DialogueWidget->InitDialogueUI(DialogueSystem, [this]() { RefreshUI(); });

      DialogueWidget->SetVisibility(ESlateVisibility::Visible);
      PriceNegotiationWidget->SetVisibility(ESlateVisibility::Hidden);
      NegotiationShowItemWidget->SetVisibility(ESlateVisibility::Hidden);
      break;
    }
    case ENegotiationState::PlayerConsider: {
      PriceNegotiationWidget->RefreshUI();

      OfferAcceptButton->SetVisibility(ESlateVisibility::Visible);
      RejectButton->SetVisibility(ESlateVisibility::Visible);

      DialogueWidget->SetVisibility(ESlateVisibility::Hidden);
      PriceNegotiationWidget->SetVisibility(ESlateVisibility::Visible);
      NegotiationShowItemWidget->SetVisibility(ESlateVisibility::Hidden);
      break;
    }

    case ENegotiationState::NpcConsider: {
      auto Dialogue = NegotiationSystem->NPCNegotiationTurn();
      DialogueWidget->InitDialogueUI(DialogueSystem, [this]() { RefreshUI(); });

      DialogueWidget->SetVisibility(ESlateVisibility::Visible);
      PriceNegotiationWidget->SetVisibility(ESlateVisibility::Hidden);
      NegotiationShowItemWidget->SetVisibility(ESlateVisibility::Hidden);
      break;
    }
    // ? Call NegotiationSuccess and NegotiationFailure?
    case ENegotiationState::Accepted:
    case ENegotiationState::Rejected:
      NegotiationSystem->NegotiationComplete();
      CloseWidgetFunc();
      break;
    default: checkNoEntry();
  }
}

void UNegotiationViewWidget::InitUI(FInputActions InputActions,
                                    const class AStore* _Store,
                                    const class AMarketEconomy* _MarketEconomy,
                                    class UInventoryComponent* _PlayerInventoryC,
                                    class UNegotiationSystem* _NegotiationSystem,
                                    class UDialogueSystem* _DialogueSystem,
                                    std::function<void()> _CloseWidgetFunc) {
  check(_Store && _MarketEconomy && _PlayerInventoryC && _NegotiationSystem && _DialogueSystem && _CloseWidgetFunc);

  Store = _Store;
  PlayerInventoryC = _PlayerInventoryC;
  NegotiationSystem = _NegotiationSystem;
  DialogueSystem = _DialogueSystem;

  DialogueWidget->InitUI(InputActions);
  ControlsHelpersWidget->SetComponentUI({
      {FText::FromString("Leave / Reject"), InputActions.CloseTopOpenMenuAction},
      {FText::FromString("Offer / Accept"), InputActions.AdvanceUIAction},
  });

  OfferAcceptButton->ActionText->SetText(FText::FromString("Offer / Accept"));
  RejectButton->ActionText->SetText(FText::FromString("Reject"));

  CloseWidgetFunc = _CloseWidgetFunc;

  // * Set initial state.
  check(NegotiationSystem->NegotiationState == ENegotiationState::None);
  auto Dialogue = NegotiationSystem->NPCRequestNegotiation();

  // When starting with a stock check, there is no item to negotiate on.
  if (NegotiationSystem->NegotiationState == ENegotiationState::NpcStockCheckRequest)
    NegotiationShowItemWidget->InitUI(
        Store, _MarketEconomy, PlayerInventoryC, NegotiationSystem, [this](UItemBase* Item) { ShowItem(Item); },
        [this]() { Reject(); });
  else PriceNegotiationWidget->InitUI(Store, NegotiationSystem);

  DialogueWidget->InitDialogueUI(DialogueSystem, [this]() {
    if (NegotiationSystem->NegotiationState == ENegotiationState::NpcRequest ||
        NegotiationSystem->NegotiationState == ENegotiationState::NpcStockCheckRequest)
      NegotiationSystem->PlayerReadRequest();
    RefreshUI();
  });

  DialogueWidget->SetVisibility(ESlateVisibility::Visible);
  PriceNegotiationWidget->SetVisibility(ESlateVisibility::Hidden);
  NegotiationShowItemWidget->SetVisibility(ESlateVisibility::Hidden);
}