#include "ChoicesBoxWidget.h"
#include "store_playground/UI/Dialogue/DialogueChoiceWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UChoicesBoxWidget::RefreshUI() {
  ChoicesBox->ClearChildren();

  for (int32 i = 0; FDialogueData Dialogue : Dialogues) {
    UDialogueChoiceWidget* DialogueChoiceWidget = CreateWidget<UDialogueChoiceWidget>(this, DialogueChoiceWidgetClass);

    DialogueChoiceWidget->InitUI(Dialogue.DialogueText, i,
                                 [this](int32 ChoiceIndex) { ChoiceSelectedFunc(ChoiceIndex); });
    ChoicesBox->AddChildToVerticalBox(DialogueChoiceWidget);

    i++;
  }
}

void UChoicesBoxWidget::InitUI(TArray<FDialogueData>& _Dialogues,
                               const FString& SpeakerName,
                               std::function<void(int32)> _ChoiceSelectedFunc) {
  check(_ChoiceSelectedFunc);

  Dialogues = _Dialogues;
  Speaker->SetText(FText::FromString(SpeakerName));
  ChoiceSelectedFunc = _ChoiceSelectedFunc;
  ChoicesBox->ClearChildren();
}