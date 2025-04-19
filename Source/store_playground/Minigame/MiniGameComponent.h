// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Minigame/MiniGameStructs.h"
#include "MiniGameComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UMiniGameComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UMiniGameComponent() {
    PrimaryComponentTick.bCanEverTick = false;
    MiniGameType = EMiniGame::Lootbox;
  }

  UPROPERTY(EditAnywhere, SaveGame)
  EMiniGame MiniGameType;
};