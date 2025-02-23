#include "DialogueChoiceWidget.h"
#include "Components/Button.h"

void UDialogueChoiceWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ChoiceButton->OnClicked.AddDynamic(this, &UDialogueChoiceWidget::ClickChoiceButton);
}

void UDialogueChoiceWidget::ClickChoiceButton() { OnChoiceSelected(); }