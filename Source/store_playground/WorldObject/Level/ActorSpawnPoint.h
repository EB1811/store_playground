// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorSpawnPoint.generated.h"

// * Subclass this in BP for any wanted spawn point type.
// The subclass acts as an identifier for the type of spawn point.

UCLASS()
class STORE_PLAYGROUND_API AActorSpawnPoint : public AActor {
  GENERATED_BODY()

public:
  AActorSpawnPoint() {
    if (!Id.IsValid()) Id = FGuid::NewGuid();

    PrimaryActorTick.bCanEverTick = false;
    SpawnChance = 0.0f;
    SpawnRadius = 0.0f;
  }

  UPROPERTY(EditAnywhere)
  FGuid Id;

  UPROPERTY(EditAnywhere)
  float SpawnChance;
  UPROPERTY(EditAnywhere)
  float SpawnRadius;

#if WITH_EDITOR
  virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override {
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (SpawnChance < 0.0f) SpawnChance = 0.0f;
    if (SpawnChance > 100.0f) SpawnChance = 100.0f;

    if (SpawnRadius < 0.0f) SpawnRadius = 0.0f;
  }
#endif
};