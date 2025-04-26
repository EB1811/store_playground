// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NpcStoreSpawnPoint.generated.h"

UCLASS()
class STORE_PLAYGROUND_API ANpcStoreSpawnPoint : public AActor {
  GENERATED_BODY()

public:
  ANpcStoreSpawnPoint() {
    if (!Id.IsValid()) Id = FGuid::NewGuid();

    PrimaryActorTick.bCanEverTick = false;
    SpawnChance = 0.0f;
  }

  UPROPERTY(EditAnywhere)
  FGuid Id;

  UPROPERTY(EditAnywhere)
  float SpawnChance;
};