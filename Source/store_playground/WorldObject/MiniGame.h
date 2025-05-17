// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MiniGame.generated.h"

UCLASS()
class STORE_PLAYGROUND_API AMiniGame : public AActor {
  GENERATED_BODY()

public:
  AMiniGame();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere)
  USceneComponent* SceneRoot;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  class UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  class UPaperFlipbookComponent* Sprite;

  UPROPERTY(EditAnywhere)
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere)
  class UDialogueComponent* DialogueComponent;

  UPROPERTY(EditAnywhere)
  class UMiniGameComponent* MiniGameComponent;

  UPROPERTY(EditAnywhere, SaveGame)
  FGuid SpawnPointId;  // * Spawn point id (for saving).
};