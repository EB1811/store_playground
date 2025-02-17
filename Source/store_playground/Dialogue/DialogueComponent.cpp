#include "DialogueComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "Engine/DataTable.h"

UDialogueComponent::UDialogueComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UDialogueComponent::BeginPlay() {
  Super::BeginPlay();

  LoadDialogueData();
}

void UDialogueComponent::LoadDialogueData() {
  DialogueArr.Empty();

  for (FDataTableRowHandle DialogueRowHandle : DialogueDataTableRows) {
    if (FDialogueDataTable* DialogueData =
            DialogueRowHandle.GetRow<FDialogueDataTable>(DialogueRowHandle.RowName.ToString())) {
      FDialogueData Data = {DialogueData->DialogueText, DialogueData->DialogueSpeaker, DialogueData->Action};
      DialogueArr.Add(Data);
    }
  }
}