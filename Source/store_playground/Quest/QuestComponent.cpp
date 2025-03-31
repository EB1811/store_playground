// Fill out your copyright notice in the Description page of Project Settings.

#include "QuestComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"

UQuestComponent::UQuestComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UQuestComponent::BeginPlay() { Super::BeginPlay(); }

// * Changes owner's interaction details, and dialgoue if needed.
void UQuestComponent::PostQuest(TArray<struct FDialogueData> PostDialogues) {
  UInteractionComponent* OwnerInteractionC = GetOwner()->FindComponentByClass<UInteractionComponent>();
  check(OwnerInteractionC);

  if (PostDialogues.Num() <= 0) {
    OwnerInteractionC->InteractionType = EInteractionType::None;
    return;
  }

  OwnerInteractionC->InteractionType = EInteractionType::NPCDialogue;

  UDialogueComponent* OwnerDialogueC = GetOwner()->FindComponentByClass<UDialogueComponent>();
  check(OwnerDialogueC);
  OwnerDialogueC->DialogueArray.Empty();
  OwnerDialogueC->DialogueArray.Append(PostDialogues);
}