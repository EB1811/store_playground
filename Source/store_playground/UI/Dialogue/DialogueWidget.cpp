#include "DialogueWidget.h"
#include "DialogueBoxWidget.h"
#include "DialogueChoiceWidget.h"
#include "HAL/Platform.h"
#include "Logging/LogMacros.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"

void UDialogueWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  DialogueBoxWidget->NextButton->OnClicked.AddDynamic(this, &UDialogueWidget::OnNext);
}

void UDialogueWidget::InitDialogueUI(UDialogueSystem* DialogueSystem) {
  check(DialogueSystem && CloseDialogueUI);
  DialogueSystemRef = DialogueSystem;

  if (DialogueSystemRef->DialogueState == EDialogueState::End) return CloseDialogueUI();

  DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Next"));

  FString SpeakerName = DialogueSystemRef->DialogueState == EDialogueState::PlayerTalk ? "Player" : "NPC";
  UpdateDialogueText(
      SpeakerName, DialogueSystemRef->DialogueDataArr[DialogueSystemRef->CurrentDialogueIndex].DialogueText,
      DialogueSystemRef->DialogueDataArr[DialogueSystemRef->CurrentDialogueIndex].Action == EDialogueAction::End);
}

// ? Change this to refresh all data?
void UDialogueWidget::UpdateDialogueText(const FString& SpeakerName, const FString& NewDialogueContent, bool IsLast) {
  DialogueBoxWidget->Speaker->SetText(FText::FromString(SpeakerName));
  DialogueBoxWidget->DialogueText->SetText(FText::FromString(NewDialogueContent));

  if (IsLast) DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Close"));
}

void UDialogueWidget::OnNext() {
  FNextDialogueRes NextDialogue = DialogueSystemRef->NextDialogue();
  if (NextDialogue.State == EDialogueState::End) return CloseDialogueUI();

  check(NextDialogue.DialogueData);

  switch (NextDialogue.State) {
    case EDialogueState::PlayerChoice: {
      DialogueBoxWidget->SetVisibility(ESlateVisibility::Hidden);
      ChoicesPanelWrapBox->ClearChildren();

      const TArray<FDialogueData>& DialogueDataArr = DialogueSystemRef->GetChoiceDialogues();
      for (int32 i = 0; FDialogueData DialogueData : DialogueDataArr) {
        UDialogueChoiceWidget* DialogueChoiceWidget =
            CreateWidget<UDialogueChoiceWidget>(this, DialogueChoiceWidgetClass);

        DialogueChoiceWidget->ChoiceIndex = i;
        DialogueChoiceWidget->ChoiceText->SetText(FText::FromString(DialogueData.DialogueText));
        DialogueChoiceWidget->OnChoiceSelected = [this, i]() { OnChoiceSelect(i); };

        ChoicesPanelWrapBox->AddChildToWrapBox(DialogueChoiceWidget);
        i++;
      }

      return;
    }
    case EDialogueState::NPCTalk:
    case EDialogueState::PlayerTalk: {
      FString SpeakerName = NextDialogue.State == EDialogueState::PlayerTalk ? "Player" : "NPC";
      UpdateDialogueText(SpeakerName, NextDialogue.DialogueData->DialogueText,
                         NextDialogue.DialogueData->Action == EDialogueAction::End);

      return;
    }
    default: check(false); return;
  }
}

void UDialogueWidget::OnChoiceSelect(int32 ChoiceIndex) {
  FNextDialogueRes NextDialogue = DialogueSystemRef->DialogueChoice(ChoiceIndex);
  if (NextDialogue.State == EDialogueState::End) return CloseDialogueUI();

  check(NextDialogue.DialogueData);

  DialogueBoxWidget->SetVisibility(ESlateVisibility::Visible);
  ChoicesPanelWrapBox->ClearChildren();

  FString SpeakerName = NextDialogue.State == EDialogueState::PlayerTalk ? "Player" : "NPC";
  UpdateDialogueText(SpeakerName, NextDialogue.DialogueData->DialogueText,
                     NextDialogue.DialogueData->Action == EDialogueAction::End);
}