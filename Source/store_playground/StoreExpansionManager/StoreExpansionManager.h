#pragma once

#include <functional>
#include "GameFramework/Info.h"
#include "store_playground/Level/LevelStructs.h"
#include "StoreExpansionManager.generated.h"

USTRUCT()
struct FStoreExpansionData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  EStoreExpansionLevel StoreExpansionLevel;

  UPROPERTY(EditAnywhere)
  FName Name;
  UPROPERTY(EditAnywhere)
  class UMaterialInstance* Picture;

  UPROPERTY(EditAnywhere)
  float Price;
  UPROPERTY(EditAnywhere)
  bool bIsLocked;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AStoreExpansionManager : public AInfo {
  GENERATED_BODY()

public:
  AStoreExpansionManager() { PrimaryActorTick.bCanEverTick = false; }

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  class AStore* Store;

  UPROPERTY(EditAnywhere)
  TArray<FStoreExpansionData> StoreExpansions;

  UPROPERTY(EditAnywhere, SaveGame)
  EStoreExpansionLevel CurrentStoreExpansionLevel;

  auto SelectExpansion(EStoreExpansionLevel StoreExpansionLevel) -> bool;
};