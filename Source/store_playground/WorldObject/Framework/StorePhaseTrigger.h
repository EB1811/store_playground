// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "StorePhaseTrigger.generated.h"

// ? Combine with customer?

UCLASS()
class STORE_PLAYGROUND_API AStorePhaseTrigger : public AActor {
  GENERATED_BODY()

public:
  AStorePhaseTrigger();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "NextLevelObject")
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere)
  class UInteractionComponent* InteractionComponent;
};