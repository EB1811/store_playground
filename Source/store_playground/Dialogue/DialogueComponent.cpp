#include "DialogueComponent.h"
#include "store_playground/Dialogue/DialogueSystem.h"
#include "Engine/DataTable.h"

UDialogueComponent::UDialogueComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UDialogueComponent::BeginPlay() { Super::BeginPlay(); }