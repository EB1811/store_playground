// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "AmbientSoundManager.generated.h"

USTRUCT()
struct FAmbientSoundManagerParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName MorningSoundTag;
  UPROPERTY(EditAnywhere)
  FName ShopOpenSoundTag;
  UPROPERTY(EditAnywhere)
  FName NightSoundTag;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AAmbientSoundManager : public AInfo {
  GENERATED_BODY()

public:
  AAmbientSoundManager() { PrimaryActorTick.bCanEverTick = false; }

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere)
  TSubclassOf<class AWorldAudio> AmbientSoundClass;

  UPROPERTY(EditAnywhere)
  FAmbientSoundManagerParams AmbientSoundManagerParams;

  void MorningSoundCalled();
  void ShopOpenSoundCalled();
  void NightSoundCalled();
};