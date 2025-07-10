// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "MusicManager.generated.h"

// ? Have a music struct (datatable?) with tags + an array holding each music type?

USTRUCT()
struct FMusicManagerParams {
  GENERATED_BODY()
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AMusicManager : public AInfo {
  GENERATED_BODY()

public:
  AMusicManager() { PrimaryActorTick.bCanEverTick = false; }

  UPROPERTY(EditAnywhere)
  FMusicManagerParams MusicManagerParams;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  class UAudioComponent* CurrentMusic;

  // * Music changes done in blueprints for fine tuning iterations.
  UFUNCTION(BlueprintImplementableEvent)
  void MorningMusicCalled();
  UFUNCTION(BlueprintImplementableEvent)
  void ShopOpenMusicCalled();
  UFUNCTION(BlueprintImplementableEvent)
  void NightMusicCalled();
};