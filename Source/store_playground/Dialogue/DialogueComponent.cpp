#include "DialogueComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "Engine/DataTable.h"

UDialogueComponent::UDialogueComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UDialogueComponent::BeginPlay() {
  Super::BeginPlay();

  LoadDialogueData();
}

void UDialogueComponent::LoadDialogueData() {
  DialogueArray.Empty();

  TArray<FDialogueDataTable*> DialogueRows;
  TableDialogues.GetRows<FDialogueDataTable>(DialogueRows, TEXT("Dialogues"));
  for (FDialogueDataTable* Row : DialogueRows)
    DialogueArray.Add({Row->DialogueChainID, Row->DialogueType, Row->DialogueText, Row->Action, Row->DialogueSpeaker,
                       Row->ChoicesAmount});
}