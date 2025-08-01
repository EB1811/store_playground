// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "store_playground/Level/LevelStructs.h"
#include "SpawnPoint.generated.h"

// * Location of spawn.

UCLASS()
class STORE_PLAYGROUND_API ASpawnPoint : public AActor {
  GENERATED_BODY()

public:
  ASpawnPoint() {
    PrimaryActorTick.bCanEverTick = false;
    Level = ELevel::None;
  }

  UPROPERTY(EditAnywhere, Category = "Spawn Point")
  ELevel Level;
};