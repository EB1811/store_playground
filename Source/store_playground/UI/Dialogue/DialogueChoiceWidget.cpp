#include "DialogueChoiceWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UDialogueChoiceWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ChoiceButton->OnClicked.AddDynamic(this, &UDialogueChoiceWidget::ClickChoice);
}

void UDialogueChoiceWidget::ClickChoice() { ChoiceSelectedFunc(ChoiceIndex); }

void UDialogueChoiceWidget::InitUI(const FString& ChoiceTextContent,
                                   int32 Index,
                                   std::function<void(int32)> _ChoiceSelectedFunc) {
  check(_ChoiceSelectedFunc);

  ChoiceText->SetText(FText::FromString(FString::Printf(TEXT("%d. %s"), Index + 1, *ChoiceTextContent)));
  ChoiceIndex = Index;
  ChoiceSelectedFunc = _ChoiceSelectedFunc;
}