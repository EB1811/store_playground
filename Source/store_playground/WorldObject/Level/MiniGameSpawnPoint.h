// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "store_playground/Minigame/MiniGameStructs.h"
#include "GameFramework/Actor.h"
#include "MiniGameSpawnPoint.generated.h"

// * Customer spawn.
// In market level, spawn customers at random spawn points.

UCLASS()
class STORE_PLAYGROUND_API AMiniGameSpawnPoint : public AActor {
  GENERATED_BODY()

public:
  AMiniGameSpawnPoint() {
    if (!Id.IsValid()) Id = FGuid::NewGuid();

    PrimaryActorTick.bCanEverTick = false;
  }

  UPROPERTY(EditAnywhere, Category = "MiniGameSpawnPoint")
  FGuid Id;

  UPROPERTY(EditAnywhere, Category = "MiniGameSpawnPoint")
  TArray<EMiniGame> MiniGameTypes;  // * MiniGame types that can spawn here.
};