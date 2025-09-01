#pragma once

#include <functional>
#include "GameFramework/Info.h"
#include "store_playground/Level/LevelStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "StoreExpansionManager.generated.h"

USTRUCT()
struct FStoreExpansionData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName StoreExpansionLevelID;

  UPROPERTY(EditAnywhere)
  FText Name;
  UPROPERTY(EditAnywhere)
  class UMaterialInstance* Picture;

  UPROPERTY(EditAnywhere, SaveGame)
  float Price;
  UPROPERTY(EditAnywhere, SaveGame)
  bool bIsLocked;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AStoreExpansionManager : public AInfo {
  GENERATED_BODY()

public:
  AStoreExpansionManager() {
    PrimaryActorTick.bCanEverTick = false;

    Upgradeable.UnlockIDs = [this](const FName DataName, const TArray<FName>& Ids) { UnlockIDs(DataName, Ids); };
  }

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  class ALevelManager* LevelManager;
  UPROPERTY(EditAnywhere)
  class AStore* Store;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FStoreExpansionData> StoreExpansions;

  UPROPERTY(EditAnywhere, SaveGame)
  FName CurrentStoreExpansionLevelID;

  void SelectExpansion(FName StoreExpansionLevelID);

  UPROPERTY(EditAnywhere)
  FUpgradeable Upgradeable;
  void UnlockIDs(const FName DataName, const TArray<FName>& Ids);
};