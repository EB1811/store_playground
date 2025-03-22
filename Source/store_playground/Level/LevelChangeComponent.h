// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Level/LevelManager.h"
#include "LevelChangeComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API ULevelChangeComponent : public UActorComponent {
  GENERATED_BODY()

public:
  ULevelChangeComponent();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere)
  ELevel LevelToLoad;
};