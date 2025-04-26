#include "CutsceneSystem.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "store_playground/Cutscene/CutsceneManager.h"

UCutsceneSystem::UCutsceneSystem() {
  StateTransitions = {
      FCStateAction{ECutsceneState::None, ECutsceneAction::StartDialogue, ECutsceneState::InDialogue},
      FCStateAction{ECutsceneState::None, ECutsceneAction::StartCutsceneAction, ECutsceneState::Waiting},

      FCStateAction{ECutsceneState::InDialogue, ECutsceneAction::StartDialogue, ECutsceneState::InDialogue},
      FCStateAction{ECutsceneState::InDialogue, ECutsceneAction::StartCutsceneAction, ECutsceneState::Waiting},
      FCStateAction{ECutsceneState::InDialogue, ECutsceneAction::FinishCutscene, ECutsceneState::Finished},

      FCStateAction{ECutsceneState::Waiting, ECutsceneAction::StartDialogue, ECutsceneState::InDialogue},
      FCStateAction{ECutsceneState::Waiting, ECutsceneAction::StartCutsceneAction, ECutsceneState::Waiting},
      FCStateAction{ECutsceneState::Waiting, ECutsceneAction::FinishCutscene, ECutsceneState::Finished},
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

void UCutsceneSystem::StartCutscene(const FResolvedCutsceneData& _ResolvedCutsceneData) {
  check(DialogueSystem && CutsceneManager);

  ResetCutscene();

  ResolvedCutsceneData = _ResolvedCutsceneData;
  CurrentCutsceneChainIndex = -1;
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
  CutsceneState = GetNextCutsceneState(CutsceneState, ECutsceneAction::StartDialogue);
  return CutsceneState;
}

auto UCutsceneSystem::PerformCutsceneChainDialogues() -> FNextDialogueRes {
  check(CutsceneState == ECutsceneState::InDialogue);
  check(CurrentCutsceneChainIndex < ResolvedCutsceneData.CutsceneChains.Num());

  const auto& CutsceneChainData = ResolvedCutsceneData.CutsceneChains[CurrentCutsceneChainIndex];
  TArray<FDialogueData> Dialogues = ResolvedCutsceneData.Dialogues.FilterByPredicate(
      [&](const FDialogueData& Dialogue) { return Dialogue.DialogueChainID == CutsceneChainData.RelevantId; });

  return DialogueSystem->StartDialogue(Dialogues);
}

void UCutsceneSystem::PerformCutsceneAction(std::function<void()> ActionFinishedFunc) {
  check(CutsceneState == ECutsceneState::Waiting);
  check(CurrentCutsceneChainIndex < ResolvedCutsceneData.CutsceneChains.Num());

  const auto& CutsceneChainData = ResolvedCutsceneData.CutsceneChains[CurrentCutsceneChainIndex];
  // * Call the CutsceneManager to perform the cutscene action.
  // Note: Not implemented.

  if (ActionFinishedFunc) ActionFinishedFunc();
}

void UCutsceneSystem::ResetCutscene() {
  CutsceneState = ECutsceneState::None;
  ResolvedCutsceneData.CutsceneChains.Empty();
  ResolvedCutsceneData.Dialogues.Empty();
  CurrentCutsceneChainIndex = -1;
}