// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/LightComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ControlledLight.generated.h"

// ? Combine with customer?

UCLASS()
class STORE_PLAYGROUND_API AControlledLight : public AActor {
  GENERATED_BODY()

public:
  AControlledLight() {
    PrimaryActorTick.bCanEverTick = false;
    LightComponent = CreateDefaultSubobject<ULightComponent>(TEXT("LightComponent"));
  }

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  ULightComponent* LightComponent;
};