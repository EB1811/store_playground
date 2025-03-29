// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Npc.generated.h"

// ? Combine with customer?

UCLASS()
class STORE_PLAYGROUND_API ANpc : public AActor {
  GENERATED_BODY()

public:
  ANpc();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Npc")
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, Category = "Npc")
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere, Category = "Npc")
  class UDialogueComponent* DialogueComponent;

  UPROPERTY(EditAnywhere, Category = "Npc")
  class UQuestComponent* QuestComponent;
};