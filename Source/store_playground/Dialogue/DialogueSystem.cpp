#include "DialogueSystem.h"
#include "DialogueDataStructs.h"
#include "DialogueComponent.h"
#include "HAL/Platform.h"

EDialogueState GetNextDialogueState(EDialogueState CurrentState, EDialogueAction Action) {
  switch (Action) {
    case EDialogueAction::NPCNext: return EDialogueState::NPCTalk;
    case EDialogueAction::PlayerNext: return EDialogueState::PlayerTalk;
    case EDialogueAction::AskPlayer: return EDialogueState::PlayerChoice;
    case EDialogueAction::End: return EDialogueState::End;
    default: return EDialogueState::None;
  }
}

// Note: Preorder traversal, only looking for direct children.
TArray<int32> GetChildChoiceIndexes(const TArray<FDialogueData>& DialogueDataArr,
                                    int32 StartIndex,
                                    int32 ChoicesAmount) {
  TArray<int32> ChoiceDialogueIndexes;
  ChoiceDialogueIndexes.Reserve(ChoicesAmount);

  int32 FoundChoices = 0;
  int32 IgnoreNext = 0;
  for (int32 i = StartIndex; i < DialogueDataArr.Num(); i += 1) {
    if (DialogueDataArr[i].DialogueType == EDialogueType::Choice) {
      if (IgnoreNext > 0) {
        IgnoreNext -= 1;
        continue;
      }

      ChoiceDialogueIndexes.Add(i);
      if (ChoiceDialogueIndexes.Num() == ChoicesAmount) break;
    }
    if (DialogueDataArr[i].Action == EDialogueAction::AskPlayer) IgnoreNext = DialogueDataArr[i].ChoicesAmount;
  }

  return ChoiceDialogueIndexes;
}

FNextDialogueRes UDialogueSystem::StartDialogue(class UDialogueComponent* _DialogueC) {
  check(_DialogueC && _DialogueC->DialogueArray.Num() > 0);

  ResetDialogue();

  DialogueC = _DialogueC;
  DialogueState =
      GetNextDialogueState(DialogueState, DialogueC->DialogueArray[0].DialogueSpeaker == EDialogueSpeaker::NPC
                                              ? EDialogueAction::NPCNext
                                              : EDialogueAction::PlayerNext);
  SpeakerName = DialogueC->SpeakerName;
  DialogueDataArr = DialogueC->GetNextDialogueChain();
  CurrentDialogueIndex = 0;
  ChoiceDialoguesSelectedIDs.Empty();

  UE_LOG(LogTemp, Warning, TEXT("StartDialogue: %d"), CurrentDialogueIndex);

  return {DialogueDataArr[CurrentDialogueIndex], DialogueState};
}

FNextDialogueRes UDialogueSystem::StartDialogue(const TArray<FDialogueData> _DialogueDataArr,
                                                const FString& _SpeakerName) {
  check(_DialogueDataArr.Num() > 0);

  ResetDialogue();

  DialogueState = GetNextDialogueState(DialogueState, _DialogueDataArr[0].DialogueSpeaker == EDialogueSpeaker::NPC
                                                          ? EDialogueAction::NPCNext
                                                          : EDialogueAction::PlayerNext);
  SpeakerName = FText::FromString(_SpeakerName);
  DialogueDataArr = _DialogueDataArr;
  CurrentDialogueIndex = 0;
  ChoiceDialoguesSelectedIDs.Empty();

  UE_LOG(LogTemp, Warning, TEXT("StartDialogue: %d"), CurrentDialogueIndex);

  return {DialogueDataArr[CurrentDialogueIndex], DialogueState};
}

FNextDialogueRes UDialogueSystem::NextDialogue() {
  DialogueState = GetNextDialogueState(DialogueState, DialogueDataArr[CurrentDialogueIndex].Action);

  switch (DialogueState) {
    case EDialogueState::NPCTalk:
    case EDialogueState::PlayerTalk: {
      CurrentDialogueIndex++;
      return {DialogueDataArr[CurrentDialogueIndex], DialogueState};
    }
    case EDialogueState::PlayerChoice: {
      return {DialogueDataArr[CurrentDialogueIndex], DialogueState};
    }
    case EDialogueState::End: {
      if (DialogueC) DialogueC->FinishReadingDialogueChain();
      return {{}, DialogueState};
    }
    default: {
      checkf(false, TEXT("Invalid Dialogue State"));
      return {{}, EDialogueState::None};
    }
  }
}

TArray<FDialogueData> UDialogueSystem::GetChoiceDialogues() {
  if (DialogueState != EDialogueState::PlayerChoice) return {};

  const TArray<int32>& ChildChoiceIndexes = GetChildChoiceIndexes(DialogueDataArr, CurrentDialogueIndex + 1,
                                                                  DialogueDataArr[CurrentDialogueIndex].ChoicesAmount);
  TArray<FDialogueData> ChoiceDialogueArr = {};
  ChoiceDialogueArr.Reserve(ChildChoiceIndexes.Num());
  for (int32 ChoiceIndex : ChildChoiceIndexes) ChoiceDialogueArr.Add(DialogueDataArr[ChoiceIndex]);

  return ChoiceDialogueArr;
}

FNextDialogueRes UDialogueSystem::DialogueChoice(int32 ChoiceIndex) {
  check(ChoiceIndex >= 0 && ChoiceIndex < DialogueDataArr[CurrentDialogueIndex].ChoicesAmount);

  if (DialogueState != EDialogueState::PlayerChoice) return {};

  const TArray<int32>& ChildChoiceIndexes = GetChildChoiceIndexes(DialogueDataArr, CurrentDialogueIndex + 1,
                                                                  DialogueDataArr[CurrentDialogueIndex].ChoicesAmount);
  int32 ChoiceDialogueIndex = ChildChoiceIndexes[ChoiceIndex];

  ChoiceDialoguesSelectedIDs.Add(DialogueDataArr[ChoiceDialogueIndex].DialogueID);
  DialogueState = GetNextDialogueState(DialogueState, DialogueDataArr[ChoiceDialogueIndex].Action);
  if (DialogueState == EDialogueState::End) return {{}, DialogueState};

  CurrentDialogueIndex = ChoiceDialogueIndex + 1;
  return {DialogueDataArr[CurrentDialogueIndex], DialogueState};
}

void UDialogueSystem::ResetDialogue() {
  DialogueC = nullptr;
  DialogueState = EDialogueState::None;
  DialogueDataArr.Empty();
  CurrentDialogueIndex = 0;
  ChoiceDialoguesSelectedIDs.Empty();
}