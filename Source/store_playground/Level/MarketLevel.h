// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "MarketLevel.generated.h"

USTRUCT()
struct FMarketLevelParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float PlayerMiscDialogueChance;  // * Chance to show a misc dialogue when entering the level.

  UPROPERTY(EditAnywhere)
  int32 RecentNpcSpawnedKeepTime;

  UPROPERTY(EditAnywhere)
  float UniqueNpcBaseSpawnChance;
};

USTRUCT()
struct FNpcStoreSaveState {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FGuid Id;

  UPROPERTY(EditAnywhere)
  TArray<struct FDialogueData> DialogueArray;
  UPROPERTY(EditAnywhere)
  TArray<TObjectPtr<class UItemBase>> ItemsArray;
  UPROPERTY(EditAnywhere)
  FNpcStoreType NpcStoreType;
};

// * Simplified save state since its not saved to disc.
USTRUCT()
struct FMarketLevelState {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  TMap<FGuid, struct FNpcStoreSaveState> NpcStoreSaveMap;

  UPROPERTY(EditAnywhere)
  TMap<FGuid, FActorSavaState> ActorSaveMap;
  UPROPERTY(EditAnywhere)
  TMap<FGuid, FComponentSaveState> ComponentSaveMap;
  UPROPERTY(EditAnywhere)
  TArray<FObjectSaveState> ObjectSaveStates;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AMarketLevel : public AInfo {
  GENERATED_BODY()

public:
  AMarketLevel();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Market Level")
  const class ASaveManager* SaveManager;
  UPROPERTY(EditAnywhere, Category = "Market Level")
  const class AGlobalDataManager* GlobalDataManager;
  UPROPERTY(EditAnywhere, Category = "Market Level")
  const class AGlobalStaticDataManager* GlobalStaticDataManager;
  UPROPERTY(EditAnywhere, Category = "Market Level")
  const class AMarket* Market;
  UPROPERTY(EditAnywhere, Category = "Market Level")
  const class AQuestManager* QuestManager;
  UPROPERTY(EditAnywhere, Category = "Market Level")
  const class AMiniGameManager* MiniGameManager;

  UPROPERTY(EditAnywhere, Category = "Market Level")
  class TSubclassOf<class ANPCStore> NPCStoreClass;
  UPROPERTY(EditAnywhere, Category = "Market Level")
  class TSubclassOf<class ANpcSpawnPoint> NpcSpawnPointClass;
  UPROPERTY(EditAnywhere, Category = "Market Level")
  class TSubclassOf<class ANpc> NpcClass;
  UPROPERTY(EditAnywhere, Category = "Market Level")
  class TSubclassOf<class AMiniGameSpawnPoint> MiniGameSpawnPointClass;
  UPROPERTY(EditAnywhere, Category = "Market Level")
  class TSubclassOf<class AMiniGame> MiniGameClass;

  UPROPERTY(EditAnywhere, Category = "Market Level")
  FMarketLevelParams LevelParams;

  UPROPERTY(EditAnywhere, Category = "Market Level")
  class APlayerCommand* PlayerCommand;

  UPROPERTY(EditAnywhere, Category = "Market Level", SaveGame)
  TMap<FName, int32> RecentlySpawnedUniqueNpcsMap;

  void EnterLevel();

  void TickDaysTimedVars();

  UPROPERTY(EditAnywhere, Category = "Market Level")
  FMarketLevelState LevelState;
  void SaveLevelState();
  void LoadLevelState();
  void ResetLevelState();

  void InitNPCStores();
  void InitMarketNpcs();
  auto TrySpawnUniqueNpc(class ANpcSpawnPoint* SpawnPoint, const FActorSpawnParameters& SpawnParams) -> bool;
  void InitMiniGames();
};