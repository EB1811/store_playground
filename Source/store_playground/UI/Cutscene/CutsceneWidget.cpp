#include "CutsceneWidget.h"
#include "store_playground/Cutscene/CutsceneSystem.h"
#include "store_playground/Cutscene/CutsceneManager.h"
#include "store_playground/Cutscene/CutsceneStructs.h"
#include "store_playground/UI/Dialogue/DialogueWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Button.h"

void UCutsceneWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  UIActionable.AdvanceUI = [this]() {
    if (CutsceneSystemRef->CutsceneState == ECutsceneState::InDialogue) DialogueWidget->OnNext();
  };
}

void UCutsceneWidget::InitUI(UCutsceneSystem* _CutsceneSystemRef) {
  check(DialogueWidget);

  this->CutsceneSystemRef = _CutsceneSystemRef;

  DialogueWidget->CloseDialogueUI = [this] { RefreshUI(); };

  RefreshUI();
}

void UCutsceneWidget::RefreshUI() {
  CutsceneSystemRef->NextCutsceneChain();

  switch (CutsceneSystemRef->CutsceneState) {
    case ECutsceneState::InDialogue: {
      auto Dialogue = CutsceneSystemRef->PerformCutsceneChainDialogues();

      // DialogueWidget->InitDialogueUI(CutsceneSystemRef->DialogueSystem);
      DialogueWidget->SetVisibility(ESlateVisibility::Visible);
      break;
    }
    case ECutsceneState::Waiting: {
      DialogueWidget->SetVisibility(ESlateVisibility::Hidden);

      CutsceneSystemRef->PerformCutsceneAction([this]() { RefreshUI(); });
      break;
    }
    case ECutsceneState::Finished: {
      CutsceneSystemRef->ResetCutscene();
      CloseThisUI();
      break;
    }
  }
}

void UCutsceneWidget::SkipCutscene() { RefreshUI(); }
