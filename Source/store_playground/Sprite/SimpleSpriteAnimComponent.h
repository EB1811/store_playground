// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AITypes.h"
#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "store_playground/Sprite/SpriteStructs.h"
#include "Components/ActorComponent.h"
#include "PaperFlipbook.h"
#include "SimpleSpriteAnimComponent.generated.h"

// * For simple sprite animations (walk, idle, etc.).

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API USimpleSpriteAnimComponent : public UActorComponent {
  GENERATED_BODY()

public:
  USimpleSpriteAnimComponent() { PrimaryComponentTick.bCanEverTick = false; }

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, SaveGame)
  TMap<ESimpleSpriteDirection, UPaperFlipbook*> IdleSprites;
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<ESimpleSpriteDirection, UPaperFlipbook*> WalkSprites;

  UPROPERTY(EditAnywhere, SaveGame)
  ESimpleSpriteAnimState SpriteAnimState;
  UPROPERTY(EditAnywhere, SaveGame)
  ESimpleSpriteDirection CurrentDirection;
  UPROPERTY(EditAnywhere, SaveGame)
  ESimpleSpriteDirection LastDirection;

  void Idle(ESimpleSpriteDirection Direction = ESimpleSpriteDirection::Down);
  void Walk(ESimpleSpriteDirection Direction = ESimpleSpriteDirection::Down);

  void MoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

  void TurnToPlayer(const FVector& PlayerLocation);
  void ReturnToOgRotation();
};