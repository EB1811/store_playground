#include "InGameHudWidget.h"
#include "Internationalization/Text.h"
#include "store_playground/UI/Components/ControlsHelpersWidget.h"
#include "store_playground/UI/Components/RightSlideWidget.h"
#include "store_playground/UI/Components/RightSlideSecondaryWidget.h"
#include "store_playground/UI/Components/RightSlideInvertedWidget.h"
#include "store_playground/UI/InGameHud/InGameHudWidgets/NewsHudSlideWidget.h"
#include "store_playground/Store/Store.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/Level/LevelManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UInGameHudWidget::InitUI(FInputActions InputActions) {
  ControlsHelpersWidget->SetComponentUI({
      {FText::FromString("News"), InputActions.OpenNewspaperAction},
      {FText::FromString("Store"), InputActions.OpenStoreViewAction},
      {FText::FromString("Items"), InputActions.OpenInventoryViewAction},
      {FText::FromString("Build"), InputActions.BuildModeAction},
  });
}

void UInGameHudWidget::RefreshUI() {
  check(NewsGen && DayManager && StorePhaseManager && Store && LevelManager);
  UE_LOG(LogTemp, Warning, TEXT("Refreshing InGame HUD UI."));

  NewsHudSlideWidget->RefreshUI();

  // Temp: Replace all with actual logic.
  FText CurrentPhaseText = UEnum::GetDisplayValueAsText(StorePhaseManager->StorePhaseState);
  int32 CurrentDay = DayManager->CurrentDay;
  DaySlideWidget->SlideText->SetText(
      FText::FromString(FString::Printf(TEXT("%s - Day: %d"), *CurrentPhaseText.ToString(), CurrentDay)));
  DaySlideWidget->RightSlideText->SetText(FText::FromString("Normal Day"));

  OwnedSlideWidget->SlideText->SetText(FText::FromString(FString::Printf(TEXT("Owned: %.0f¬"), 8000.0f)));
  OwnedSlideWidget->RightSlideText->SetText(FText::FromString(FString::Printf(TEXT("On Day: %d"), 30)));

  const float Money = Store->Money;
  MoneySlideWidget->SlideText->SetText(FText::FromString(FString::Printf(TEXT("Money: %.0f¬"), Money)));
  MoneySlideWidget->RightSlideText->SetText(FText::FromString(""));

  FText CurrentLevelText = UEnum::GetDisplayValueAsText(LevelManager->LoadedLevel);
  LocationSlideWidget->SlideText->SetText(CurrentLevelText);
}
