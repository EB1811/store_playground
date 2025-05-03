// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "StorePGGameInstance.generated.h"

USTRUCT()
struct FGameSettings {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  bool bShowTutorials;
};

UCLASS()
class STORE_PLAYGROUND_API UStorePGGameInstance : public UGameInstance {
  GENERATED_BODY()

public:
  UStorePGGameInstance();

  UPROPERTY(EditAnywhere)
  bool bFromSaveGame;  // * Whether the game was loaded from a save game or not.
  UPROPERTY(EditAnywhere)
  int32 SaveSlotIndex;  // * Index of the save slot to load from.

  UPROPERTY(EditAnywhere)
  bool bFromGameOver;  // * Whether the game was loaded from a game over or not.

  UPROPERTY(EditAnywhere)
  FGameSettings GameSettings;
};
