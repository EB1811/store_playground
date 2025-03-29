// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomerSpawnPoint.generated.h"

// * Location of spawn.
// ? Also use for other spawn points (customer, etc.)?

UCLASS()
class STORE_PLAYGROUND_API ACustomerSpawnPoint : public AActor {
  GENERATED_BODY()

public:
  ACustomerSpawnPoint() {
    PrimaryActorTick.bCanEverTick = false;
    SpawnChance = 0.0f;
  }

  UPROPERTY()
  float SpawnChance;
};