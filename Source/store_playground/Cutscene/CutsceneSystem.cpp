#include "CutsceneSystem.h"
#include "Logging/LogVerbosity.h"
#include "Misc/AssertionMacros.h"
#include "TimerManager.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/Cutscene/CutsceneManager.h"
#include "store_playground/UI/SpgHUD.h"

UCutsceneSystem::UCutsceneSystem() {
  StateTransitions = {
      FCStateAction{ECutsceneState::None, ECutsceneAction::StartDialogue, ECutsceneState::InDialogue},
      FCStateAction{ECutsceneState::None, ECutsceneAction::StartCutsceneAction, ECutsceneState::WaitingForAction},

      FCStateAction{ECutsceneState::InDialogue, ECutsceneAction::StartDialogue, ECutsceneState::InDialogue},
      FCStateAction{ECutsceneState::InDialogue, ECutsceneAction::StartCutsceneAction, ECutsceneState::WaitingForAction},
      FCStateAction{ECutsceneState::InDialogue, ECutsceneAction::FinishCutscene, ECutsceneState::Finished},

      FCStateAction{ECutsceneState::WaitingForAction, ECutsceneAction::StartDialogue, ECutsceneState::InDialogue},
      FCStateAction{ECutsceneState::WaitingForAction, ECutsceneAction::StartCutsceneAction,
                    ECutsceneState::WaitingForAction},
      FCStateAction{ECutsceneState::WaitingForAction, ECutsceneAction::FinishCutscene, ECutsceneState::Finished},
  };

  CutsceneState = ECutsceneState::None;
  CurrentCutsceneChainIndex = 0;
}

ECutsceneState UCutsceneSystem::GetNextCutsceneState(ECutsceneState State, ECutsceneAction Action) {
  for (const struct FCStateAction& Transition : StateTransitions)
    if (Transition.initial == State && Transition.action == Action) return Transition.next;

  checkf(false, TEXT("Invalid Action %d for State %d"), (int)Action, (int)State);

  return ECutsceneState::None;
}

void UCutsceneSystem::StartCutscene(const FResolvedCutsceneData& _ResolvedCutsceneData,
                                    std::function<void()> _CutsceneFinishedFunc) {
  check(DialogueSystem && CutsceneManager && _CutsceneFinishedFunc);

  ResetCutscene();

  ResolvedCutsceneData = _ResolvedCutsceneData;
  CutsceneFinishedFunc = _CutsceneFinishedFunc;

  HUD->HideInGameHudWidget();
  HandleCutsceneState();
}

void UCutsceneSystem::HandleCutsceneState() {
  NextCutsceneChain();

  switch (CutsceneState) {
    case ECutsceneState::InDialogue: {
      PerformCutsceneChainDialogues();
      HUD->SetAndOpenCutsceneDialogue(DialogueSystem, [this]() { HandleCutsceneState(); });
      break;
    }
    case ECutsceneState::WaitingForAction: {
      PerformCutsceneAction();
      break;
    }
    case ECutsceneState::Finished: {
      HUD->ShowInGameHudWidget();
      CutsceneFinishedFunc();
      break;
    }
  }
}

auto UCutsceneSystem::NextCutsceneChain() -> ECutsceneState {
  CurrentCutsceneChainIndex++;
  if (CurrentCutsceneChainIndex >= ResolvedCutsceneData.CutsceneChains.Num()) {
    CutsceneState = GetNextCutsceneState(CutsceneState, ECutsceneAction::FinishCutscene);
    return CutsceneState;
  }

  const auto CutsceneChainData = ResolvedCutsceneData.CutsceneChains[CurrentCutsceneChainIndex];

  ECutsceneAction Action = CutsceneChainData.CutsceneChainType == ECutsceneChainType::Dialogue
                               ? ECutsceneAction::StartDialogue
                               : ECutsceneAction::StartCutsceneAction;
  CutsceneState = GetNextCutsceneState(CutsceneState, Action);

  return CutsceneState;
}
void UCutsceneSystem::SkipCutsceneChain() {
  check(CutsceneState == ECutsceneState::InDialogue || CutsceneState == ECutsceneState::WaitingForAction);

  switch (CutsceneState) {
    case ECutsceneState::InDialogue: {
      // * Quiting the ui will run the CloseDialogueFunc, which will call HandleCutsceneState.
      HUD->QuitUIAction();
      break;
    }
    case ECutsceneState::WaitingForAction: {
      // HandleCutsceneState();
      break;
    }
  }
}

auto UCutsceneSystem::PerformCutsceneChainDialogues() -> FNextDialogueRes {
  check(CutsceneState == ECutsceneState::InDialogue);
  check(CurrentCutsceneChainIndex < ResolvedCutsceneData.CutsceneChains.Num());

  const auto& CutsceneChainData = ResolvedCutsceneData.CutsceneChains[CurrentCutsceneChainIndex];
  TArray<FDialogueData> Dialogues = ResolvedCutsceneData.Dialogues.FilterByPredicate(
      [&](const FDialogueData& Dialogue) { return Dialogue.DialogueChainID == CutsceneChainData.RelevantId; });
  check(Dialogues.Num() > 0);

  return DialogueSystem->StartDialogue(Dialogues);
}

void UCutsceneSystem::PerformCutsceneAction(std::function<void()> ActionFinishedFunc) {
  check(CutsceneState == ECutsceneState::WaitingForAction);
  check(CurrentCutsceneChainIndex < ResolvedCutsceneData.CutsceneChains.Num());

  const auto& CutsceneChainData = ResolvedCutsceneData.CutsceneChains[CurrentCutsceneChainIndex];
  // * Call the CutsceneManager to perform the cutscene action.
  // Note: Not implemented.

  // Just waiting for a few seconds to simulate the cutscene action.
  FTimerDelegate Delegate = FTimerDelegate::CreateLambda([this]() {
    if (!HUD->DialogueWidget) return;

    HandleCutsceneState();
  });
  GetWorld()->GetTimerManager().SetTimer(ActionTimer, Delegate, 2.0f, false);
}

void UCutsceneSystem::PauseCutscene() {
  if (CutsceneState == ECutsceneState::WaitingForAction) GetWorld()->GetTimerManager().PauseTimer(ActionTimer);
}
void UCutsceneSystem::ResumeCutscene() {
  if (CutsceneState == ECutsceneState::WaitingForAction) GetWorld()->GetTimerManager().UnPauseTimer(ActionTimer);
}
void UCutsceneSystem::ResetCutscene() {
  CutsceneState = ECutsceneState::None;
  ResolvedCutsceneData.CutsceneChains.Empty();
  ResolvedCutsceneData.Dialogues.Empty();
  CurrentCutsceneChainIndex = -1;
  CutsceneFinishedFunc = nullptr;
  GetWorld()->GetTimerManager().ClearTimer(ActionTimer);
}