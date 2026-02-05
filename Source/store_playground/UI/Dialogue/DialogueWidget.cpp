#include "DialogueWidget.h"
#include "Components/Border.h"
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

  return DialogueSystem->DialogueState == EDialogueState::PlayerTalk ||
                 DialogueSystem->DialogueState == EDialogueState::PlayerChoice
             ? FText::FromString("You")
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
void UDialogueWidget::UpdateDialogueText(const FText& SpeakerName, const FText& NewDialogueContent, bool IsLast) {
  DialogueBoxWidget->Speaker->SetText(SpeakerName);
  DialogueBoxWidget->DialogueText->SetText(NewDialogueContent);

  if (IsLast) DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Close"));
}
void UDialogueWidget::SetDialogueSpeakerMaterial(FName SpeakerID) {
  if (DialogueSystem->DialogueState == EDialogueState::PlayerTalk ||
      DialogueSystem->DialogueState == EDialogueState::PlayerChoice)
    return DialogueBoxWidget->BgBorder->SetBrushFromMaterial(PlayerSpeakerMaterial);

  UMaterialInstance** FoundMaterial = SpeakerMaterialMap.Find(SpeakerID);
  if (FoundMaterial) DialogueBoxWidget->BgBorder->SetBrushFromMaterial(*FoundMaterial);
  else DialogueBoxWidget->BgBorder->SetBrushFromMaterial(DefaultSpeakerMaterial);
}
void UDialogueWidget::UpdateDialogueBasedOnState() {
  FDialogueData CurrDialogue = DialogueSystem->DialogueDataArr[DialogueSystem->CurrentDialogueIndex];
  switch (DialogueSystem->DialogueState) {
    case EDialogueState::PlayerChoice: {
      FText SpeakerName = GetSpeakerName(DialogueSystem, CurrDialogue);
      auto Dialogues = DialogueSystem->GetChoiceDialogues();

      ChoicesBoxWidget->InitUI(Dialogues, SpeakerName, [this](int32 ChoiceIndex) { SelectChoice(ChoiceIndex); });
      ChoicesBoxWidget->RefreshUI();

      DialogueBoxWidget->SetVisibility(ESlateVisibility::Hidden);
      ChoicesBoxWidget->SetVisibility(ESlateVisibility::Visible);

      break;
    }
    case EDialogueState::PlayerInquire: {
      FText SpeakerName = GetSpeakerName(DialogueSystem, CurrDialogue);
      auto Dialogues = DialogueSystem->GetInquireDialogues();

      ChoicesBoxWidget->InitUI(Dialogues, SpeakerName, [this](int32 InquireIndex) { SelectInquire(InquireIndex); });
      ChoicesBoxWidget->RefreshUI();

      DialogueBoxWidget->SetVisibility(ESlateVisibility::Hidden);
      ChoicesBoxWidget->SetVisibility(ESlateVisibility::Visible);

      break;
    }
    case EDialogueState::NPCTalk:
    case EDialogueState::PlayerTalk: {
      FText SpeakerName = GetSpeakerName(DialogueSystem, CurrDialogue);
      UpdateDialogueText(SpeakerName, CurrDialogue.DialogueText, CurrDialogue.Action == EDialogueAction::End);
      SetDialogueSpeakerMaterial(FName(SpeakerName.ToString()));

      DialogueBoxWidget->SetVisibility(ESlateVisibility::Visible);
      ChoicesBoxWidget->SetVisibility(ESlateVisibility::Collapsed);

      break;
    }
    case EDialogueState::End: {
      return CloseDialogueFunc(true);
    }
    default: checkNoEntry(); break;
  }
}

void UDialogueWidget::Next() {
  if (DialogueSystem->DialogueState != EDialogueState::PlayerTalk &&
      DialogueSystem->DialogueState != EDialogueState::NPCTalk)
    return;

  DialogueSystem->NextDialogue();
  UpdateDialogueBasedOnState();

  UGameplayStatics::PlaySound2D(this, NextSound, 1.0f);
}

void UDialogueWidget::SelectChoice(int32 ChoiceIndex) {
  check(DialogueSystem->DialogueState == EDialogueState::PlayerChoice && ChoiceIndex >= 0 &&
        ChoiceIndex < DialogueSystem->GetChoiceDialogues().Num());

  DialogueSystem->DialogueChoice(ChoiceIndex);
  UpdateDialogueBasedOnState();

  UGameplayStatics::PlaySound2D(this, NextSound, 1.0f);
}
void UDialogueWidget::SelectInquire(int32 InquireIndex) {
  check(DialogueSystem->DialogueState == EDialogueState::PlayerInquire && InquireIndex >= 0 &&
        InquireIndex < DialogueSystem->GetInquireDialogues().Num());

  DialogueSystem->InquireDialogue(InquireIndex);
  UpdateDialogueBasedOnState();

  UGameplayStatics::PlaySound2D(this, NextSound, 1.0f);
}

void UDialogueWidget::InitUI() {
  DialogueBoxWidget->NextButtonText->SetText(FText::FromString("Next"));

  UpdateDialogueBasedOnState();
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
  UIActionable.NumericInput = [this](float Value) {
    if (DialogueSystem->DialogueState == EDialogueState::PlayerChoice) SelectChoice(FMath::RoundToInt(Value) - 1);
    else if (DialogueSystem->DialogueState == EDialogueState::PlayerInquire)
      SelectInquire(FMath::RoundToInt(Value) - 1);
  };
  UIActionable.RetractUI = [this]() { CloseDialogueFunc(false); };
  UIActionable.QuitUI = [this]() { CloseDialogueFunc(false); };
}
void UDialogueWidget::SetupUIBehaviour() {
  UIBehaviour.ShowAnim = ShowAnim;
  UIBehaviour.HideAnim = HideAnim;
  UIBehaviour.OpenSound = OpenSound;
}