// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NpcSpawnPoint.generated.h"

// * Customer spawn.
// In market level, spawn customers at random spawn points.

UCLASS()
class STORE_PLAYGROUND_API ANpcSpawnPoint : public AActor {
  GENERATED_BODY()

public:
  ANpcSpawnPoint() {
    PrimaryActorTick.bCanEverTick = false;
    SpawnChance = 0.0f;
  }

  UPROPERTY(EditAnywhere, Category = "Spawn Point")
  float SpawnChance;
};