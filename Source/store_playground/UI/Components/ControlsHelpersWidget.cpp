#include "ControlsHelpersWidget.h"
#include "ControlTextWidget.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void UControlsHelpersWidget::SetComponentUI(const TArray<FControls>& _Controls) {
  check(ControlTextWidgetClass);

  Controls = _Controls;

  ControlsWrapBox->ClearChildren();
  for (const FControls& Control : Controls) {
    if (UControlTextWidget* ControlTextWidget = CreateWidget<UControlTextWidget>(GetWorld(), ControlTextWidgetClass)) {
      ControlTextWidget->ActionText->SetText(Control.ActionName);
      ControlTextWidget->CommonActionWidget->SetEnhancedInputAction(Control.ActionBinding);
      // ControlTextWidget->ActionBindingText->SetText(FText::FromString("(" + Control.ActionBinding.ToString() + ")"));
      ControlsWrapBox->AddChildToWrapBox(ControlTextWidget);
    }
  }
}
