// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StorePGGameMode.generated.h"

/**
 *
 */
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
  TSubclassOf<class AStorePhaseManager> StorePhaseManagerClass;
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
};
