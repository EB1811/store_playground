// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StorePGGameMode.generated.h"

UCLASS()
class STORE_PLAYGROUND_API AStorePGGameMode : public AGameModeBase {
  GENERATED_BODY()

public:
  AStorePGGameMode();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class ALevelManager> LevelManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AGlobalDataManager> GlobalDataManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AGlobalStaticDataManager> GlobalStaticDataManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AUpgradeManager> UpgradeManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class ASaveManager> SaveManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class APlayerCommand> PlayerCommandClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AStorePhaseManager> StorePhaseManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class ADayManager> DayManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AStorePhaseLightingManager> StorePhaseLightingManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AMusicManager> MusicManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AStoreExpansionManager> StoreExpansionManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class ACutsceneManager> CutsceneManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class ACustomerAIManager> CustomerAIManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AQuestManager> QuestManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AMarket> MarketClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AMarketEconomy> MarketEconomyClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class ANewsGen> NewsGenClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AStore> StoreClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AMiniGameManager> MiniGameManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AMarketLevel> MarketLevelClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AAbilityManager> AbilityManagerClass;
  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class AStatisticsGen> StatisticsGenClass;

  UPROPERTY(EditAnywhere, Category = "BP Classes")
  TSubclassOf<class ASpawnPoint> SpawnPointClass;  // * To spawn the player in the store level.

  // * Storing some properties to be used in the post level load init.
  UPROPERTY()
  class ASaveManager* SaveManager;
  UPROPERTY()
  class AStorePhaseManager* StorePhaseManager;

  UFUNCTION(BlueprintCallable, Category = "GameMode")
  void GameOverReset();
  UFUNCTION(BlueprintCallable, Category = "GameMode")
  void LoadGame(int32 SlotIndex);
};
