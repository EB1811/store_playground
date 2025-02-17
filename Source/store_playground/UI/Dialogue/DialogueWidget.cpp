#include "DialogueWidget.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UDialogueWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  NextButton->OnClicked.AddDynamic(this, &UDialogueWidget::OnNextButtonClicked);
}

void UDialogueWidget::RefreshDialogueWhole() {
  check(DialogueSystemRef);

  if (DialogueSystemRef->DialogueState == EDialogueState::End) {
    if (CloseDialogueUI != nullptr) CloseDialogueUI();
    return;
  }

  FString SpeakerName = DialogueSystemRef->DialogueState == EDialogueState::PlayerTalk ? "Player" : "NPC";
  UpdateDialogueText(SpeakerName,
                     DialogueSystemRef->DialogueDataArr[DialogueSystemRef->CurrentDialogueIndex].DialogueText);
}

void UDialogueWidget::UpdateDialogueText(const FString& SpeakerName, const FString& NewDialogueContent) {
  Speaker->SetText(FText::FromString(SpeakerName));
  DialogueText->SetText(FText::FromString(NewDialogueContent));
}

void UDialogueWidget::OnNextButtonClicked() {
  NextDialogueRes NextDialogue = DialogueSystemRef->NextDialogue();
  if (NextDialogue.State == EDialogueState::End) {
    CloseDialogueUI();
    return;
  }

  FString SpeakerName = NextDialogue.State == EDialogueState::PlayerTalk ? "Player" : "NPC";
  UpdateDialogueText(SpeakerName, NextDialogue.DialogueText);
}