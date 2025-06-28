#include "PriceNegotiationWidget.h"
#include "store_playground/UI/Components/RightSlideWidget.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Negotiation/PriceSliderWidget.h"
#include "store_playground/UI/Inventory/CompactItemDetailsWidget.h"
#include "store_playground/UI/Negotiation/NegotiationSkillsWidget.h"
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

void UPriceNegotiationWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

void UPriceNegotiationWidget::RefreshUI() {
  MoneySlideWidget->SlideText->SetText(FText::FromString(FString::Printf(TEXT("Money: %.0f¬"), Store->Money)));

  float PlayerPrice = NegotiationSystem->OfferedPrice;
  float NpcPrice = NegotiationSystem->CustomerOfferResponse.CounterOffer > 0.0f
                       ? NegotiationSystem->CustomerOfferResponse.CounterOffer
                       : NegotiationSystem->MarketPrice;
  PriceSliderWidget->UpdateNegotiationPrices(PlayerPrice, NpcPrice);
}

void UPriceNegotiationWidget::InitUI(const AStore* _Store, UNegotiationSystem* _NegotiationSystem) {
  check(_Store && _NegotiationSystem);

  Store = _Store;

  NegotiationSystem = _NegotiationSystem;

  float NpcAcceptance = NegotiationSystem->CustomerAI->NegotiationAI->AcceptancePercentage;
  float MarketPrice = NegotiationSystem->MarketPrice;
  float PlayerPrice = NegotiationSystem->OfferedPrice;
  float NpcPrice = NegotiationSystem->CustomerOfferResponse.CounterOffer > 0.0f
                       ? NegotiationSystem->CustomerOfferResponse.CounterOffer
                       : MarketPrice;
  float BoughtAtPrice = NegotiationSystem->BoughtAtPrice;
  PriceSliderWidget->InitUI(NegotiationSystem->Type, NpcAcceptance, MarketPrice, PlayerPrice, NpcPrice, BoughtAtPrice);

  CompactItemDetailsWidget->InitUI(NegotiationSystem->NegotiatedItems[0], "Bought At:", MarketPrice, BoughtAtPrice);
}