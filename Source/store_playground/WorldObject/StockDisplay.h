// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StockDisplay.generated.h"

UCLASS()
class STORE_PLAYGROUND_API AStockDisplay : public AActor {
  GENERATED_BODY()

public:
  AStockDisplay();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "StockDisplay")
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, Category = "StockDisplay")
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere, Category = "StockDisplay")
  class UInventoryComponent* InventoryComponent;

  UPROPERTY(EditAnywhere, Category = "StockDisplay")
  class UStockDisplayComponent* StockDisplayComponent;
};