// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "ChurchLevel.generated.h"

USTRUCT()
struct FChurchLevelParams {
  GENERATED_BODY()
};

USTRUCT()
struct FChurchLevelState {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  TMap<FGuid, FActorSavaState> ActorSaveMap;
  UPROPERTY(EditAnywhere)
  TMap<FGuid, FComponentSaveState> ComponentSaveMap;
  UPROPERTY(EditAnywhere)
  TArray<FObjectSaveState> ObjectSaveStates;

  UPROPERTY(EditAnywhere)
  TArray<FGuid> PersistentIds;  // * Since above maps can be cleared each day.
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AChurchLevel : public AInfo {
  GENERATED_BODY()

public:
  AChurchLevel();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  class TSubclassOf<class APickupActor> PickupActorClass;

  UPROPERTY(EditAnywhere)
  FChurchLevelParams LevelParams;

  UPROPERTY(EditAnywhere)
  const class ASaveManager* SaveManager;

  UPROPERTY(EditAnywhere)
  class APlayerCommand* PlayerCommand;

  void EnterLevel();

  UPROPERTY(EditAnywhere)
  FChurchLevelState LevelState;
  void SaveLevelState();
  void LoadLevelState();
  void ResetDaysLevelState();
};