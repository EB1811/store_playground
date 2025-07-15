#include "ControlMenuButtonWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UControlMenuButtonWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  // check(ClickedSound); // ! This is failing but the value is set?
}

void UControlMenuButtonWidget::SetActiveStyle(bool bActive) {
  if (bActive) ControlButton->SetBackgroundColor(ActiveColor);
  else ControlButton->SetBackgroundColor(DefaultColor);
}