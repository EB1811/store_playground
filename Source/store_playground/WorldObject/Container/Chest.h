// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chest.generated.h"

UCLASS()
class STORE_PLAYGROUND_API AChest : public AActor
{
  GENERATED_BODY()

public:
  AChest();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Chest")
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, Category = "Chest")
  class UInventoryComponent* InventoryComponent;

  UPROPERTY(EditAnywhere, Category = "Chest")
  class UInteractionComponent* InteractionComponent;
};