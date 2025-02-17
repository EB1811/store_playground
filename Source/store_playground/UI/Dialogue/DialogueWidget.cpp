#include "DialogueWidget.h"
#include "DialogueBoxWidget.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UDialogueWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  DialogueBoxWidget->NextButton->OnClicked.AddDynamic(this, &UDialogueWidget::OnNext);
}

void UDialogueWidget::InitDialogueUI() {
  check(DialogueSystemRef);

  if (DialogueSystemRef->DialogueState == EDialogueState::End) {
    if (CloseDialogueUI != nullptr) CloseDialogueUI();
    return;
  }

  DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Next"));

  FString SpeakerName = DialogueSystemRef->DialogueState == EDialogueState::PlayerTalk ? "Player" : "NPC";
  UpdateDialogueText(
      SpeakerName, DialogueSystemRef->DialogueDataArr[DialogueSystemRef->CurrentDialogueIndex].DialogueText,
      DialogueSystemRef->DialogueDataArr[DialogueSystemRef->CurrentDialogueIndex].Action == EDialogueAction::End);
}

void UDialogueWidget::UpdateDialogueText(const FString& SpeakerName, const FString& NewDialogueContent, bool IsLast) {
  DialogueBoxWidget->Speaker->SetText(FText::FromString(SpeakerName));
  DialogueBoxWidget->DialogueText->SetText(FText::FromString(NewDialogueContent));

  if (IsLast) DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Close"));
}

void UDialogueWidget::OnNext() {
  NextDialogueRes NextDialogue = DialogueSystemRef->NextDialogue();
  if (NextDialogue.State == EDialogueState::End) {
    CloseDialogueUI();
    return;
  }

  FString SpeakerName = NextDialogue.State == EDialogueState::PlayerTalk ? "Player" : "NPC";
  UpdateDialogueText(SpeakerName, NextDialogue.DialogueData.DialogueText,
                     NextDialogue.DialogueData.Action == EDialogueAction::End);
}