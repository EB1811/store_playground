// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Npc.generated.h"

// ? Combine with customer?
// TODO: Turn into character.

UCLASS()
class STORE_PLAYGROUND_API ANpc : public APaperZDCharacter {
  GENERATED_BODY()

public:
  ANpc();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Npc")
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere, Category = "Npc")
  class UDialogueComponent* DialogueComponent;

  UPROPERTY(EditAnywhere, Category = "Npc")
  class UQuestComponent* QuestComponent;

  UPROPERTY(EditAnywhere, Category = "Npc", SaveGame)
  FGuid SpawnPointId;  // * Spawn point id (for saving).
};