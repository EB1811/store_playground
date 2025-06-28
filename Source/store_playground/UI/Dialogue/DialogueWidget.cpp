#include "DialogueWidget.h"
#include "DialogueBoxWidget.h"
#include "DialogueChoiceWidget.h"
#include "HAL/Platform.h"
#include "Logging/LogMacros.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/UI/Components/ControlsHelpersWidget.h"
#include "store_playground/UI/Dialogue/ChoicesBoxWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"

auto GetSpeakerName(const UDialogueSystem* DialogueSystem) -> FString {
  return DialogueSystem->DialogueState == EDialogueState::PlayerTalk ? "Player"
                                                                     : DialogueSystem->SpeakerName.ToString();
}

void UDialogueWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  DialogueBoxWidget->NextButton->OnClicked.AddDynamic(this, &UDialogueWidget::OnNext);

  UIActionable.AdvanceUI = [this]() { OnNext(); };

  UIBehaviour.ShowUI = [this]() { OnVisibilityChangeRequested(ESlateVisibility::Visible); };
  UIBehaviour.HideUI = [this]() { OnVisibilityChangeRequested(ESlateVisibility::Collapsed); };
}

void UDialogueWidget::InitDialogueUI(UDialogueSystem* _DialogueSystem, std::function<void()> _CloseDialogueUI) {
  check(_DialogueSystem && _CloseDialogueUI);
  DialogueBoxWidget->SetVisibility(ESlateVisibility::Visible);
  ChoicesBoxWidget->SetVisibility(ESlateVisibility::Collapsed);

  DialogueSystem = _DialogueSystem;
  CloseDialogueUI = _CloseDialogueUI;

  if (DialogueSystem->DialogueState == EDialogueState::End) return CloseDialogueUI();

  DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Next"));

  FString SpeakerName = GetSpeakerName(DialogueSystem);
  UpdateDialogueText(
      SpeakerName, DialogueSystem->DialogueDataArr[DialogueSystem->CurrentDialogueIndex].DialogueText,
      DialogueSystem->DialogueDataArr[DialogueSystem->CurrentDialogueIndex].Action == EDialogueAction::End);
}

// ? Change this to refresh all data?
void UDialogueWidget::UpdateDialogueText(const FString& SpeakerName, const FString& NewDialogueContent, bool IsLast) {
  DialogueBoxWidget->Speaker->SetText(FText::FromString(SpeakerName));
  DialogueBoxWidget->DialogueText->SetText(FText::FromString(NewDialogueContent));

  if (IsLast) DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Close"));
}

void UDialogueWidget::OnNext() {
  FNextDialogueRes NextDialogue = DialogueSystem->NextDialogue();
  if (NextDialogue.State == EDialogueState::End) return CloseDialogueUI();

  check(NextDialogue.DialogueData);

  switch (NextDialogue.State) {
    case EDialogueState::PlayerChoice: {
      FString SpeakerName = GetSpeakerName(DialogueSystem);
      auto Dialogues = DialogueSystem->GetChoiceDialogues();

      ChoicesBoxWidget->InitUI(Dialogues, SpeakerName, [this](int32 ChoiceIndex) { OnChoiceSelect(ChoiceIndex); });
      ChoicesBoxWidget->RefreshUI();

      DialogueBoxWidget->SetVisibility(ESlateVisibility::Hidden);
      ChoicesBoxWidget->SetVisibility(ESlateVisibility::Visible);

      return;
    }
    case EDialogueState::NPCTalk:
    case EDialogueState::PlayerTalk: {
      FString SpeakerName = GetSpeakerName(DialogueSystem);
      UpdateDialogueText(SpeakerName, NextDialogue.DialogueData->DialogueText,
                         NextDialogue.DialogueData->Action == EDialogueAction::End);

      return;
    }
    default: check(false); return;
  }
}

void UDialogueWidget::OnChoiceSelect(int32 ChoiceIndex) {
  FNextDialogueRes NextDialogue = DialogueSystem->DialogueChoice(ChoiceIndex);
  if (NextDialogue.State == EDialogueState::End) return CloseDialogueUI();

  check(NextDialogue.DialogueData);

  DialogueBoxWidget->SetVisibility(ESlateVisibility::Visible);
  ChoicesBoxWidget->SetVisibility(ESlateVisibility::Collapsed);

  FString SpeakerName = GetSpeakerName(DialogueSystem);
  UpdateDialogueText(SpeakerName, NextDialogue.DialogueData->DialogueText,
                     NextDialogue.DialogueData->Action == EDialogueAction::End);
}

void UDialogueWidget::InitUI(FInputActions InputActions) {
  ControlsHelpersWidget->SetComponentUI({
      {FText::FromString("Leave"), InputActions.CloseTopOpenMenuAction},
      {FText::FromString("Next"), InputActions.AdvanceUIAction},
  });
}