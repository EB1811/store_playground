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

  UPROPERTY(EditAnywhere, Category = "MiniGame")
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, Category = "MiniGame")
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere, Category = "MiniGame")
  class UDialogueComponent* DialogueComponent;

  UPROPERTY(EditAnywhere, Category = "MiniGame")
  class UMiniGameComponent* MiniGameComponent;

  UPROPERTY(EditAnywhere, Category = "MiniGame", SaveGame)
  FGuid SpawnPointId;  // * Spawn point id (for saving).
};