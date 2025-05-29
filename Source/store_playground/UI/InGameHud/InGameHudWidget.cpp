#include "InGameHudWidget.h"
#include "store_playground/UI/Components/ControlsHelpersWidget.h"
#include "store_playground/UI/InGameHud/InGameHudWidgets/NewsHudSlideWidget.h"

void UInGameHudWidget::InitUI(FInputActions InputActions) {
  // * Set the controls for the HUD.
  ControlsHelpersWidget->SetComponentUI({
      {FText::FromString("News"), InputActions.OpenNewspaperAction},
      {FText::FromString("Store"), InputActions.OpenStatisticsAction},
      {FText::FromString("Items"), InputActions.OpenInventoryViewAction},
      {FText::FromString("Build"), InputActions.BuildModeAction},
  });
}

void UInGameHudWidget::RefreshUI() { NewsHudSlideWidget->RefreshUI(); }
