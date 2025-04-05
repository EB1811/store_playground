// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UpgradePoint.generated.h"

UCLASS()
class STORE_PLAYGROUND_API AUpgradePoint : public AActor {
  GENERATED_BODY()

public:
  AUpgradePoint();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "UpgradePoint")
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, Category = "UpgradePoint")
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere, Category = "UpgradePoint")
  class UUpgradeSelectComponent* UpgradeSelectComponent;
};