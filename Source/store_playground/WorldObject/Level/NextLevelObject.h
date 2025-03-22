// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NextLevelObject.generated.h"

UCLASS()
class STORE_PLAYGROUND_API ANextLevelObject : public AActor {
  GENERATED_BODY()

public:
  ANextLevelObject();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "NextLevelObject")
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, Category = "NextLevelObject")
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere, Category = "NextLevelObject")
  class ULevelChangeComponent* LevelChangeComponent;
};