// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StockDisplayComponent.generated.h"

USTRUCT()
struct FDisplayStats {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  int32 PickRateWeightModifier;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UStockDisplayComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UStockDisplayComponent();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Stock Display")
  FDisplayStats DisplayStats;
};