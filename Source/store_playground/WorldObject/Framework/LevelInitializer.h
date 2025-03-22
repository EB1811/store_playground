// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "LevelInitializer.generated.h"

// ! Workaround for https://forums.unrealengine.com/t/get-all-actors-of-class-with-level-streaming-broken-after-update-to-4-9/330584.
// * Finds and calls needed persistent actors' functions.
// ! Not used for now.

UENUM()
enum class ELevelInitializerType : uint8 {
  None,
  Store,
  Market,
  //
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ALevelInitializer : public AInfo {
  GENERATED_BODY()

public:
  ALevelInitializer();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Level Initializer")
  ELevelInitializerType LevelInitializerType;

  UPROPERTY(EditAnywhere, Category = "Level Initializer")
  TSubclassOf<class AActor> RelevantActorClass;

  void InitializeStoreLevel();
  void InitializeMarketLevel();
};