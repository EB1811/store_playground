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

auto GetSpeakerName(const UDialogueSystem* DialogueSystem) -> FString {
  return DialogueSystem->DialogueState == EDialogueState::PlayerTalk ? "Player"
                                                                     : DialogueSystem->SpeakerName.ToString();
}

void UDialogueWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  check(NextSound);

  DialogueBoxWidget->NextButton->OnClicked.AddDynamic(this, &UDialogueWidget::Next);

  SetupUIActionable();
  SetupUIBehaviour();
}

// ? Change this to refresh all data?
void UDialogueWidget::UpdateDialogueText(const FString& SpeakerName, const FString& NewDialogueContent, bool IsLast) {
  DialogueBoxWidget->Speaker->SetText(FText::FromString(SpeakerName));
  DialogueBoxWidget->DialogueText->SetText(FText::FromString(NewDialogueContent));

  if (IsLast) DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Close"));
}

void UDialogueWidget::Next() {
  if (DialogueSystem->DialogueState != EDialogueState::PlayerTalk &&
      DialogueSystem->DialogueState != EDialogueState::NPCTalk)
    return;

  FNextDialogueRes NextDialogue = DialogueSystem->NextDialogue();
  if (NextDialogue.State == EDialogueState::End) {
    CloseDialogueFunc();
    if (FinishDialogueFunc) FinishDialogueFunc();
    return;
  }

  check(NextDialogue.DialogueData);
  switch (NextDialogue.State) {
    case EDialogueState::PlayerChoice: {
      FString SpeakerName = GetSpeakerName(DialogueSystem);
      auto Dialogues = DialogueSystem->GetChoiceDialogues();

      ChoicesBoxWidget->InitUI(Dialogues, SpeakerName, [this](int32 ChoiceIndex) { SelectChoice(ChoiceIndex); });
      ChoicesBoxWidget->RefreshUI();

      DialogueBoxWidget->SetVisibility(ESlateVisibility::Hidden);
      ChoicesBoxWidget->SetVisibility(ESlateVisibility::Visible);

      break;
    }
    case EDialogueState::NPCTalk:
    case EDialogueState::PlayerTalk: {
      FString SpeakerName = GetSpeakerName(DialogueSystem);
      UpdateDialogueText(SpeakerName, NextDialogue.DialogueData->DialogueText,
                         NextDialogue.DialogueData->Action == EDialogueAction::End);

      break;
    }
    default: checkNoEntry(); break;
  }

  UGameplayStatics::PlaySound2D(this, NextSound, 0.75f);
}

void UDialogueWidget::SelectChoice(int32 ChoiceIndex) {
  if (DialogueSystem->DialogueState != EDialogueState::PlayerChoice || ChoiceIndex < 0 ||
      ChoiceIndex >= DialogueSystem->GetChoiceDialogues().Num())
    return;

  FNextDialogueRes NextDialogue = DialogueSystem->DialogueChoice(ChoiceIndex);
  if (NextDialogue.State == EDialogueState::End) {
    CloseDialogueFunc();
    if (FinishDialogueFunc) FinishDialogueFunc();
    return;
  }

  check(NextDialogue.DialogueData);

  DialogueBoxWidget->SetVisibility(ESlateVisibility::Visible);
  ChoicesBoxWidget->SetVisibility(ESlateVisibility::Collapsed);

  FString SpeakerName = GetSpeakerName(DialogueSystem);
  UpdateDialogueText(SpeakerName, NextDialogue.DialogueData->DialogueText,
                     NextDialogue.DialogueData->Action == EDialogueAction::End);

  UGameplayStatics::PlaySound2D(this, NextSound, 0.75f);
}

void UDialogueWidget::InitUI(FInUIInputActions InUIInputActions,
                             UDialogueSystem* _DialogueSystem,
                             std::function<void()> _CloseDialogueFunc,
                             std::function<void()> _FinishDialogueFunc) {
  check(_DialogueSystem && _CloseDialogueFunc);

  DialogueSystem = _DialogueSystem;
  FinishDialogueFunc = _FinishDialogueFunc;
  CloseDialogueFunc = _CloseDialogueFunc;

  ControlsHelpersWidget->SetComponentUI({
      {FText::FromString("Leave"), InUIInputActions.RetractUIAction},
      {FText::FromString("Next"), InUIInputActions.AdvanceUIAction},
  });

  if (DialogueSystem->DialogueState == EDialogueState::End) {
    CloseDialogueFunc();
    if (FinishDialogueFunc) FinishDialogueFunc();
    return;
  }

  DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Next"));

  FString SpeakerName = GetSpeakerName(DialogueSystem);
  UpdateDialogueText(
      SpeakerName, DialogueSystem->DialogueDataArr[DialogueSystem->CurrentDialogueIndex].DialogueText,
      DialogueSystem->DialogueDataArr[DialogueSystem->CurrentDialogueIndex].Action == EDialogueAction::End);

  DialogueBoxWidget->SetVisibility(ESlateVisibility::Visible);
  ChoicesBoxWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UDialogueWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { Next(); };
  UIActionable.NumericInput = [this](float Value) { SelectChoice(FMath::RoundToInt(Value) - 1); };
  UIActionable.RetractUI = [this]() { CloseDialogueFunc(); };
  UIActionable.QuitUI = [this]() { CloseDialogueFunc(); };
}

void UDialogueWidget::SetupUIBehaviour() {
  FWidgetAnimationDynamicEvent UIAnimEvent;
  UIAnimEvent.BindDynamic(this, &UDialogueWidget::UIAnimComplete);
  BindToAnimationFinished(ShowAnim, UIAnimEvent);
  BindToAnimationFinished(HideAnim, UIAnimEvent);

  UIBehaviour.ShowUI = [this](std::function<void()> Callback) {
    UIAnimCompleteFunc = Callback;

    SetVisibility(ESlateVisibility::Visible);
    UUserWidget::PlayAnimation(ShowAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
  };
  UIBehaviour.HideUI = [this](std::function<void()> Callback) {
    UIAnimCompleteFunc = [this, Callback]() {
      if (Callback) Callback();

      SetVisibility(ESlateVisibility::Collapsed);
    };

    UUserWidget::PlayAnimation(HideAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
  };
}
void UDialogueWidget::UIAnimComplete() {
  if (UIAnimCompleteFunc) UIAnimCompleteFunc();
}