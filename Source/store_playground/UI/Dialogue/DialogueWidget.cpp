#include "DialogueWidget.h"
#include "DialogueBoxWidget.h"
#include "DialogueChoiceWidget.h"
#include "HAL/Platform.h"
#include "Logging/LogMacros.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/UI/Components/ControlsHelpersWidget.h"
#include "store_playground/UI/Dialogue/ChoicesBoxWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"

auto GetSpeakerName(const UDialogueSystem* DialogueSystem, FDialogueData& Dialogue) -> FText {
  if (!Dialogue.SpeakerName.IsEmptyOrWhitespace()) return Dialogue.SpeakerName;

  return DialogueSystem->DialogueState == EDialogueState::PlayerTalk ? FText::FromString("You")
                                                                     : DialogueSystem->SpeakerName;
}

void UDialogueWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  check(NextSound);

  DialogueBoxWidget->NextButton->OnClicked.AddDynamic(this, &UDialogueWidget::Next);

  SetupUIActionable();
  SetupUIBehaviour();
}

// ? Change this to refresh all data?
void UDialogueWidget::UpdateDialogueText(const FText& SpeakerName, const FString& NewDialogueContent, bool IsLast) {
  DialogueBoxWidget->Speaker->SetText(SpeakerName);
  DialogueBoxWidget->DialogueText->SetText(FText::FromString(NewDialogueContent));

  if (IsLast) DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Close"));
}

void UDialogueWidget::Next() {
  if (DialogueSystem->DialogueState != EDialogueState::PlayerTalk &&
      DialogueSystem->DialogueState != EDialogueState::NPCTalk)
    return;

  FNextDialogueRes NextDialogue = DialogueSystem->NextDialogue();
  if (NextDialogue.State == EDialogueState::End) return CloseDialogueFunc(true);

  check(NextDialogue.DialogueData);
  switch (NextDialogue.State) {
    case EDialogueState::PlayerChoice: {
      FText SpeakerName = GetSpeakerName(DialogueSystem, NextDialogue.DialogueData.GetValue());
      auto Dialogues = DialogueSystem->GetChoiceDialogues();

      ChoicesBoxWidget->InitUI(Dialogues, SpeakerName, [this](int32 ChoiceIndex) { SelectChoice(ChoiceIndex); });
      ChoicesBoxWidget->RefreshUI();

      DialogueBoxWidget->SetVisibility(ESlateVisibility::Hidden);
      ChoicesBoxWidget->SetVisibility(ESlateVisibility::Visible);

      break;
    }
    case EDialogueState::NPCTalk:
    case EDialogueState::PlayerTalk: {
      FText SpeakerName = GetSpeakerName(DialogueSystem, NextDialogue.DialogueData.GetValue());
      UpdateDialogueText(SpeakerName, NextDialogue.DialogueData->DialogueText,
                         NextDialogue.DialogueData->Action == EDialogueAction::End);

      break;
    }
    default: checkNoEntry(); break;
  }

  UGameplayStatics::PlaySound2D(this, NextSound, 1.0f);
}

void UDialogueWidget::SelectChoice(int32 ChoiceIndex) {
  if (DialogueSystem->DialogueState != EDialogueState::PlayerChoice || ChoiceIndex < 0 ||
      ChoiceIndex >= DialogueSystem->GetChoiceDialogues().Num())
    return;

  FNextDialogueRes NextDialogue = DialogueSystem->DialogueChoice(ChoiceIndex);
  if (NextDialogue.State == EDialogueState::End) return CloseDialogueFunc(true);

  check(NextDialogue.DialogueData);

  DialogueBoxWidget->SetVisibility(ESlateVisibility::Visible);
  ChoicesBoxWidget->SetVisibility(ESlateVisibility::Collapsed);

  FText SpeakerName = GetSpeakerName(DialogueSystem, NextDialogue.DialogueData.GetValue());
  UpdateDialogueText(SpeakerName, NextDialogue.DialogueData->DialogueText,
                     NextDialogue.DialogueData->Action == EDialogueAction::End);

  UGameplayStatics::PlaySound2D(this, NextSound, 1.0f);
}

void UDialogueWidget::InitUI() {
  if (DialogueSystem->DialogueState == EDialogueState::End) return CloseDialogueFunc(true);

  DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Next"));

  switch (DialogueSystem->DialogueState) {
    case EDialogueState::PlayerChoice: {
      FText SpeakerName =
          GetSpeakerName(DialogueSystem, DialogueSystem->DialogueDataArr[DialogueSystem->CurrentDialogueIndex]);
      auto Dialogues = DialogueSystem->GetChoiceDialogues();

      ChoicesBoxWidget->InitUI(Dialogues, SpeakerName, [this](int32 ChoiceIndex) { SelectChoice(ChoiceIndex); });
      ChoicesBoxWidget->RefreshUI();

      DialogueBoxWidget->SetVisibility(ESlateVisibility::Hidden);
      ChoicesBoxWidget->SetVisibility(ESlateVisibility::Visible);

      break;
    }
    case EDialogueState::NPCTalk:
    case EDialogueState::PlayerTalk: {
      FText SpeakerName =
          GetSpeakerName(DialogueSystem, DialogueSystem->DialogueDataArr[DialogueSystem->CurrentDialogueIndex]);
      UpdateDialogueText(
          SpeakerName, DialogueSystem->DialogueDataArr[DialogueSystem->CurrentDialogueIndex].DialogueText,
          DialogueSystem->DialogueDataArr[DialogueSystem->CurrentDialogueIndex].Action == EDialogueAction::End);

      DialogueBoxWidget->SetVisibility(ESlateVisibility::Visible);
      ChoicesBoxWidget->SetVisibility(ESlateVisibility::Collapsed);

      break;
    }
    default: checkNoEntry(); break;
  }
}
void UDialogueWidget::InitUI(FInUIInputActions InputActions,
                             UDialogueSystem* _DialogueSystem,
                             std::function<void(bool)> _CloseDialogueFunc) {
  check(_DialogueSystem && _CloseDialogueFunc);

  DialogueSystem = _DialogueSystem;
  CloseDialogueFunc = _CloseDialogueFunc;

  ControlsHelpersWidget->SetComponentUI({
      {FText::FromString("Leave"), InputActions.RetractUIAction},
      {FText::FromString("Next"), InputActions.AdvanceUIAction},
  });

  InitUI();
}
void UDialogueWidget::InitUI(FInCutsceneInputActions InputActions,
                             UDialogueSystem* _DialogueSystem,
                             std::function<void(bool)> _CloseDialogueFunc) {
  check(_DialogueSystem && _CloseDialogueFunc);

  DialogueSystem = _DialogueSystem;
  CloseDialogueFunc = _CloseDialogueFunc;

  ControlsHelpersWidget->SetComponentUI({
      {FText::FromString("Skip"), InputActions.SkipCutsceneAction},
      {FText::FromString("Next"), InputActions.AdvanceCutsceneAction},
  });

  InitUI();
}

void UDialogueWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { Next(); };
  UIActionable.NumericInput = [this](float Value) { SelectChoice(FMath::RoundToInt(Value) - 1); };
  UIActionable.RetractUI = [this]() { CloseDialogueFunc(false); };
  UIActionable.QuitUI = [this]() { CloseDialogueFunc(false); };
}
void UDialogueWidget::SetupUIBehaviour() {
  UIBehaviour.ShowAnim = ShowAnim;
  UIBehaviour.HideAnim = HideAnim;
  UIBehaviour.OpenSound = OpenSound;
}