// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "MusicManager.generated.h"

USTRUCT()
struct FMusicManagerParams {
  GENERATED_BODY()
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AMusicManager : public AInfo {
  GENERATED_BODY()

public:
  AMusicManager() { PrimaryActorTick.bCanEverTick = false; }

  UPROPERTY(EditAnywhere, Category = "Music")
  FMusicManagerParams MusicManagerParams;

  // * Music changes done in blueprints for fine tuning iterations.
  UFUNCTION(BlueprintImplementableEvent)
  void OnEndDayMusicCalled();
  UFUNCTION(BlueprintImplementableEvent)
  void OnOpenShopMusicCalled();
  UFUNCTION(BlueprintImplementableEvent)
  void OnCloseShopMusicCalled();
};