// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

UCLASS()
class STORE_PLAYGROUND_API AMainMenuGameMode : public AGameModeBase {
  GENERATED_BODY()

public:
  AMainMenuGameMode() { CheckCount = 0; };

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class ASettingsManager> SettingsManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class ASaveManager> SaveManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AMusicManager> MusicManagerClass;

  UPROPERTY(EditAnywhere)
  class AMainMenuControlHUD* MainMenuControlHUD;
  UPROPERTY(EditAnywhere)
  class ASaveManager* SaveManager;
  UPROPERTY(EditAnywhere)
  class AMusicManager* MusicManager;

  UPROPERTY()
  int32 CheckCount;
  FTimerHandle CheckLoadCompleteTimerHandle;
  UFUNCTION()
  void CheckLoadComplete();  // Completes a series of checks to ensure everything is loaded, loops until done.

  UFUNCTION(BlueprintCallable, Category = "GameMode")
  void Continue();
  UFUNCTION(BlueprintCallable, Category = "GameMode")
  void StartNewGame();
  UFUNCTION(BlueprintCallable, Category = "GameMode")
  void LoadGame(int32 SlotIndex);
  UFUNCTION(BlueprintCallable, Category = "GameMode")
  void Exit();
};
