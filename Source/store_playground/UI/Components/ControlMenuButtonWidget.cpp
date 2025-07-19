#include "ControlMenuButtonWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UControlMenuButtonWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

void UControlMenuButtonWidget::SetActiveStyle(bool bActive) {
  if (bActive) ControlButton->SetBackgroundColor(ActiveColor);
  else ControlButton->SetBackgroundColor(DefaultColor);
}