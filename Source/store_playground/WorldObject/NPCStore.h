// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NPCStore.generated.h"

UCLASS()
class STORE_PLAYGROUND_API ANPCStore : public AActor {
  GENERATED_BODY()

public:
  ANPCStore();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "NPCStore")
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, Category = "NPCStore")
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere, Category = "NPCStore")
  class UDialogueComponent* DialogueComponent;

  UPROPERTY(EditAnywhere, Category = "NPCStore")
  class UInventoryComponent* InventoryComponent;

  UPROPERTY(EditAnywhere, Category = "NPCStore")
  class UNpcStoreComponent* NpcStoreComponent;
};