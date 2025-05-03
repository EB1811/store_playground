#include "DialogueComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "Engine/DataTable.h"

void UDialogueComponent::BeginPlay() { Super::BeginPlay(); }

auto UDialogueComponent::GetNextDialogueChain() -> TArray<FDialogueData> {
  switch (DialogueComponentType) {
    case EDialogueComponentType::Simple: {
      return DialogueArray;
    }
    case EDialogueComponentType::Rolling: {
      if (DialogueArray.Num() == 0) return {};

      if (CurrentDialogueChainId.IsNone()) CurrentDialogueChainId = DialogueArray[0].DialogueChainID;

      TArray<FDialogueData> NextDialogueChain;
      for (int32 i = 0; i < DialogueArray.Num(); i++)
        if (DialogueArray[i].DialogueChainID == CurrentDialogueChainId) NextDialogueChain.Add(DialogueArray[i]);

      check(NextDialogueChain.Num() > 0);
      return NextDialogueChain;
    }
    case EDialogueComponentType::Random: {
      if (DialogueArray.Num() == 0) return {};

      CurrentDialogueChainId = DialogueArray[FMath::RandRange(0, DialogueArray.Num() - 1)].DialogueChainID;

      TArray<FDialogueData> NextDialogueChain;
      for (int32 i = 0; i < DialogueArray.Num(); i++)
        if (DialogueArray[i].DialogueChainID == CurrentDialogueChainId) NextDialogueChain.Add(DialogueArray[i]);

      check(NextDialogueChain.Num() > 0);
      return NextDialogueChain;
    }
    default: checkNoEntry(); return {};
  }
}

void UDialogueComponent::FinishReadingDialogueChain() {
  if (DialogueComponentType == EDialogueComponentType::Rolling) {
    int32 CurrentDialogueChainEndIndex = DialogueArray.IndexOfByPredicate(
        [this](const FDialogueData& Dialogue) { return Dialogue.DialogueChainID == CurrentDialogueChainId; });
    for (int32 i = CurrentDialogueChainEndIndex; i < DialogueArray.Num(); i++)
      if (DialogueArray[i].DialogueChainID == CurrentDialogueChainId) CurrentDialogueChainEndIndex = i;
      else break;

    if (CurrentDialogueChainEndIndex >= DialogueArray.Num() - 1) return;  // Final chain reached.

    CurrentDialogueChainId = DialogueArray[CurrentDialogueChainEndIndex + 1].DialogueChainID;
  }
}