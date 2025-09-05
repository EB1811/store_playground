// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "store_playground/Pickup/PickupComponent.h"
#include "PickupActor.generated.h"

UCLASS()
class STORE_PLAYGROUND_API APickupActor : public AActor {
  GENERATED_BODY()

public:
  APickupActor();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  FGuid Id;

  UPROPERTY(EditAnywhere)
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere)
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere)
  class UPickupComponent* PickupComponent;
};