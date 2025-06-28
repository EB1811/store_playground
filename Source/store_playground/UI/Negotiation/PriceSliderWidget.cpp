#include "PriceSliderWidget.h"
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

void UPriceSliderWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  PlayerPriceSlider->OnValueChanged.AddDynamic(this, &UPriceSliderWidget::UpdatePlayerPriceText);
}

void UPriceSliderWidget::UpdatePlayerPriceText(float Value) {
  PlayerPriceText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(Value))));

  UCanvasPanelSlot* PlayerSliderBoxAsCanvas = Cast<UCanvasPanelSlot>(PlayerSliderBox->Slot);
  auto PlayerSliderBoxSize = PlayerSliderBoxAsCanvas->GetSize().Y;
  PlayerPriceText->SetRenderTranslation(
      FVector2D(0.0f, PlayerSliderBoxSize - PlayerPriceSlider->GetNormalizedValue() * PlayerSliderBoxSize -
                          PlayerPriceText->GetDesiredSize().Y / 2.0f));
}

void UPriceSliderWidget::UpdateNegotiationPrices(float PlayerPrice, float NpcPrice) {
  PlayerPriceSlider->SetValue(PlayerPrice);
  NPCPriceSlider->SetValue(NpcPrice);

  NPCPriceText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(NpcPrice))));
  PlayerPriceText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(PlayerPrice))));

  UCanvasPanelSlot* PlayerSliderBoxAsCanvas = Cast<UCanvasPanelSlot>(PlayerSliderBox->Slot);
  auto PlayerSliderBoxSize = PlayerSliderBoxAsCanvas->GetSize().Y;
  NPCPriceText->SetRenderTranslation(FVector2D(0.0f, PlayerSliderBoxSize -
                                                         NPCPriceSlider->GetNormalizedValue() * PlayerSliderBoxSize -
                                                         PlayerPriceText->GetDesiredSize().Y / 2.0f));
  PlayerPriceText->SetRenderTranslation(
      FVector2D(0.0f, PlayerSliderBoxSize - PlayerPriceSlider->GetNormalizedValue() * PlayerSliderBoxSize -
                          PlayerPriceText->GetDesiredSize().Y / 2.0f));
}

void UPriceSliderWidget::InitUI(NegotiationType _Type,
                                float NpcAcceptance,
                                float MarketPrice,
                                float PlayerPrice,
                                float NpcPrice,
                                float BoughtAtPrice) {
  Type = _Type;
  switch (Type) {
    case NegotiationType::PlayerBuy:
      YouTopBox->SetVisibility(ESlateVisibility::Collapsed);
      YouBottomBox->SetVisibility(ESlateVisibility::Visible);
      ThemTopBox->SetVisibility(ESlateVisibility::Visible);
      ThemBottomBox->SetVisibility(ESlateVisibility::Collapsed);
      break;
    case NegotiationType::PlayerSell:
      YouTopBox->SetVisibility(ESlateVisibility::Visible);
      YouBottomBox->SetVisibility(ESlateVisibility::Collapsed);
      ThemTopBox->SetVisibility(ESlateVisibility::Collapsed);
      ThemBottomBox->SetVisibility(ESlateVisibility::Visible);
      break;
    default: checkNoEntry()
  }

  float StepSize = FMath::RoundToInt(MarketPrice * 0.01f);
  float MaxValue = MarketPrice * ((NpcAcceptance - 1.0f) * PriceSliderUIParams.AcceptanceBarMulti + 1.0f);
  float MinValue = MarketPrice / ((NpcAcceptance - 1.0f) * PriceSliderUIParams.AcceptanceBarMulti + 1.0f);

  MarketPriceSlider->SetStepSize(StepSize);
  MarketPriceSlider->SetMaxValue(MaxValue);
  MarketPriceSlider->SetMinValue(MinValue);
  PlayerPriceSlider->SetStepSize(StepSize);
  PlayerPriceSlider->SetMaxValue(MaxValue);
  PlayerPriceSlider->SetMinValue(MinValue);
  NPCPriceSlider->SetStepSize(StepSize);
  NPCPriceSlider->SetMaxValue(MaxValue);
  NPCPriceSlider->SetMinValue(MinValue);
  BoughtAtSlider->SetStepSize(StepSize);
  BoughtAtSlider->SetMaxValue(MaxValue);
  BoughtAtSlider->SetMinValue(MinValue);

  MarketPriceSlider->SetValue(MarketPrice);
  PlayerPriceSlider->SetValue(PlayerPrice);
  NPCPriceSlider->SetValue(NpcPrice);
  if (BoughtAtPrice > 0.0f) BoughtAtSlider->SetValue(BoughtAtPrice);
  else BoughtAtSlider->SetVisibility(ESlateVisibility::Collapsed);

  switch (Type) {
    case NegotiationType::PlayerBuy:
      RedBar->SetBarFillType(EProgressBarFillType::TopToBottom);
      YellowBar->SetBarFillType(EProgressBarFillType::TopToBottom);
      GreenBar->SetBarFillType(EProgressBarFillType::TopToBottom);

      RedBar->SetPercent(1.0f);
      GreenBar->SetPercent(1.0f - MarketPriceSlider->GetNormalizedValue());
      YellowBar->SetPercent(
          1.0f - FMath::Max((MarketPrice * (1.0f - (NpcAcceptance - 1.0f)) - MinValue) / (MaxValue - MinValue), 0.0f));
      break;
    case NegotiationType::PlayerSell:
      RedBar->SetBarFillType(EProgressBarFillType::BottomToTop);
      YellowBar->SetBarFillType(EProgressBarFillType::BottomToTop);
      GreenBar->SetBarFillType(EProgressBarFillType::BottomToTop);

      RedBar->SetPercent(1.0f);
      GreenBar->SetPercent(MarketPriceSlider->GetNormalizedValue());
      YellowBar->SetPercent((MarketPrice * NpcAcceptance - MinValue) / (MaxValue - MinValue));
      break;
    default: checkNoEntry()
  }

  NPCPriceText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(NpcPrice))));
  PlayerPriceText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(PlayerPrice))));
  // ! GetDesiredSize() is not updated yet, so a hardcoded value of 50.0f is used here.
  // Can do a delay, or somehow force it to update.
  UCanvasPanelSlot* PlayerSliderBoxAsCanvas = Cast<UCanvasPanelSlot>(PlayerSliderBox->Slot);
  auto PlayerSliderBoxSize = PlayerSliderBoxAsCanvas->GetSize().Y;
  NPCPriceText->SetRenderTranslation(
      FVector2D(0.0f, PlayerSliderBoxSize - NPCPriceSlider->GetNormalizedValue() * PlayerSliderBoxSize - 50.0f / 2.0f));
  PlayerPriceText->SetRenderTranslation(FVector2D(
      0.0f, PlayerSliderBoxSize - PlayerPriceSlider->GetNormalizedValue() * PlayerSliderBoxSize - 50.0f / 2.0f));
}