// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "GameplayTagContainer.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "CutsceneManager.generated.h"

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ACutsceneManager : public AInfo {
  GENERATED_BODY()

public:
  ACutsceneManager() { PrimaryActorTick.bCanEverTick = false; }

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  const class AGlobalStaticDataManager* GlobalStaticDataManager;

  UPROPERTY(EditAnywhere)
  class UTagsComponent* PlayerTags;
  UPROPERTY(EditAnywhere)
  class APlayerCommand* PlayerCommand;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FName> PlayedCutscenes;

  auto PlayPotentialCutscene(FGameplayTagContainer& CallerCutsceneTags) -> bool;
};