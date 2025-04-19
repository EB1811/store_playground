// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilityPoint.generated.h"

UCLASS()
class STORE_PLAYGROUND_API AAbilityPoint : public AActor {
  GENERATED_BODY()

public:
  AAbilityPoint();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "AbilityPoint")
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, Category = "AbilityPoint")
  class UInteractionComponent* InteractionComponent;
};