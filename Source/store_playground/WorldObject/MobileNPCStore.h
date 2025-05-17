// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbookComponent.h"
#include "MobileNPCStore.generated.h"

UCLASS()
class STORE_PLAYGROUND_API AMobileNPCStore : public AActor {
  GENERATED_BODY()

public:
  AMobileNPCStore();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "NPCStore")
  FGuid NpcStoreId;

  UPROPERTY(EditAnywhere, Category = "NPCStore")
  USceneComponent* SceneRoot;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPCStore")
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPCStore")
  UPaperFlipbookComponent* Sprite;

  UPROPERTY(EditAnywhere, Category = "NPCStore")
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere, Category = "NPCStore")
  class UDialogueComponent* DialogueComponent;

  UPROPERTY(EditAnywhere, Category = "NPCStore")
  class UInventoryComponent* InventoryComponent;

  UPROPERTY(EditAnywhere, Category = "NPCStore")
  class UNpcStoreComponent* NpcStoreComponent;

  UPROPERTY(EditAnywhere, Category = "Npc", SaveGame)
  FGuid SpawnPointId;
};