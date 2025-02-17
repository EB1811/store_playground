#include "DialogueSystem.h"

EDialogueState GetNextDialogueState(EDialogueState CurrentState, EDialogueAction Action) {
  switch (Action) {
    case EDialogueAction::NPCNext: return EDialogueState::NPCTalk;
    case EDialogueAction::PlayerNext: return EDialogueState::PlayerTalk;
    case EDialogueAction::AskPlayer: return EDialogueState::PlayerChoice;
    case EDialogueAction::End: return EDialogueState::End;
    default: return EDialogueState::None;
  }
}

NextDialogueRes UDialogueSystem::StartDialogue(const TArray<FDialogueData> _DialogueDataArr) {
  check(_DialogueDataArr.Num() > 0);

  DialogueState = GetNextDialogueState(DialogueState, _DialogueDataArr[0].DialogueSpeaker == EDialogueSpeaker::NPC
                                                          ? EDialogueAction::NPCNext
                                                          : EDialogueAction::PlayerNext);
  CurrentDialogueIndex = 0;
  DialogueDataArr = _DialogueDataArr;

  return {DialogueDataArr[CurrentDialogueIndex], DialogueState};
}

NextDialogueRes UDialogueSystem::NextDialogue() {
  DialogueState = GetNextDialogueState(DialogueState, DialogueDataArr[CurrentDialogueIndex].Action);

  switch (DialogueState) {
    case EDialogueState::NPCTalk:
    case EDialogueState::PlayerTalk:
    case EDialogueState::PlayerChoice: {
      CurrentDialogueIndex++;
      return {DialogueDataArr[CurrentDialogueIndex], DialogueState};
    }
    case EDialogueState::End: return {{}, DialogueState};
    default: return {{}, EDialogueState::None};
  }
}
