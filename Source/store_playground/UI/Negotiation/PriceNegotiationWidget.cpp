#include "PriceNegotiationWidget.h"
#include "store_playground/UI/Components/RightSlideWidget.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Components/ControlsHelpersWidget.h"
#include "store_playground/UI/Negotiation/PriceSliderWidget.h"
#include "store_playground/UI/Inventory/CompactItemDetailsWidget.h"
#include "store_playground/UI/Negotiation/NegotiationSkillsWidget.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Ability/AbilityManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/AI/CustomerAIManager.h"
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
#include "store_playground/Upgrade/UpgradeStructs.h"

void UPriceNegotiationWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SetupUIActionable();
}

void UPriceNegotiationWidget::OfferAcceptPrice() {
  float Price = PriceSliderWidget->PlayerPriceSlider->GetValue();
  check(Price >= 0.0f);

  if (NegotiationSystem->Type == NegotiationType::PlayerBuy && Price > Store->GetAvailableMoney()) return;

  OfferAcceptFunc(Price);
}

void UPriceNegotiationWidget::RefreshUI() {
  MoneySlideWidget->SlideText->SetText(FText::FromString(FString::Printf(TEXT("Money: %.0f¬"), Store->Money)));
  MoneySlideWidget->RightSlideText->SetText(FText::FromString(""));

  float NpcAcceptance = NegotiationSystem->CustomerAI->NegotiationAIDetails.AcceptancePercentage / 100.0f;
  float MarketPrice = NegotiationSystem->MarketPrice;
  float PlayerPrice = NegotiationSystem->OfferedPrice;
  float NpcPrice = NegotiationSystem->CustomerOfferResponse.CounterOffer > 0.0f
                       ? NegotiationSystem->CustomerOfferResponse.CounterOffer
                       : NegotiationSystem->MarketPrice;
  PriceSliderWidget->UpdateNegotiationPrices(NpcAcceptance, MarketPrice, PlayerPrice, NpcPrice,
                                             NegotiationSystem->CustomerAI->NegotiationAIDetails.HagglingCount);
}

void UPriceNegotiationWidget::InitUI(FInUIInputActions InUIInputActions,
                                     const AAbilityManager* _AbilityManager,
                                     const AStore* _Store,
                                     const class ACustomerAIManager* _CustomerAIManager,
                                     UNegotiationSystem* _NegotiationSystem,
                                     std::function<void(float)> _OfferAcceptFunc,
                                     std::function<void()> _RejectFunc) {
  check(_AbilityManager && _Store && _NegotiationSystem && _OfferAcceptFunc && _RejectFunc);

  AbilityManager = _AbilityManager;
  Store = _Store;
  CustomerAIManager = _CustomerAIManager;
  NegotiationSystem = _NegotiationSystem;
  OfferAcceptFunc = _OfferAcceptFunc;
  RejectFunc = _RejectFunc;

  check(NegotiationSystem->NegotiatedItems.Num() > 0);

  float NpcAcceptance = NegotiationSystem->CustomerAI->NegotiationAIDetails.AcceptancePercentage / 100.0f;
  float MarketPrice = NegotiationSystem->MarketPrice;
  float PlayerPrice = NegotiationSystem->OfferedPrice;
  float NpcPrice = NegotiationSystem->CustomerOfferResponse.CounterOffer > 0.0f
                       ? NegotiationSystem->CustomerOfferResponse.CounterOffer
                       : MarketPrice;
  float BoughtAtPrice = NegotiationSystem->BoughtAtPrice;
  PriceSliderWidget->InitUI(NegotiationSystem, CustomerAIManager, NegotiationSystem->Type, NpcAcceptance, MarketPrice,
                            PlayerPrice, NpcPrice, Store->GetAvailableMoney(), BoughtAtPrice);

  CompactItemDetailsWidget->InitUI(NegotiationSystem->NegotiatedItems[0], "Bought At:", MarketPrice, BoughtAtPrice);

  NegotiationSkillsWidget->InitUI(AbilityManager->ActiveNegotiationSkills);

  ControlsHelpersWidget->SetComponentUI({
      {FText::FromString("Leave / Reject"), InUIInputActions.RetractUIAction},
      {FText::FromString("Offer / Accept"), InUIInputActions.AdvanceUIAction},
      {FText::FromString("Precise Input On / Off"), InUIInputActions.UISideButton4Action},
  });
}

void UPriceNegotiationWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { OfferAcceptPrice(); };
  UIActionable.DirectionalInput = [this](FVector2D Direction) { PriceSliderWidget->ChangePrice(Direction.X); };
  UIActionable.SideButton4 = [this]() {
    PriceSliderWidget->TogglePreciseInput();  // temp: Using a simple toggle for now.
  };
  UIActionable.RetractUI = [this]() { RejectFunc(); };
}