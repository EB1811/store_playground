// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/AudioComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldAudio.generated.h"

UCLASS()
class STORE_PLAYGROUND_API AWorldAudio : public AActor {
  GENERATED_BODY()

public:
  AWorldAudio() {
    // PrimaryActorTick.bCanEverTick = false;
    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
  }

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UAudioComponent* AudioComponent;
};