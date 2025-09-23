#pragma once

#include <functional>
#include "GameFramework/Info.h"
#include "store_playground/Level/LevelStructs.h"
#include "LevelManager.generated.h"

// * Responsible for loading and unloading levels dynamically.

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ALevelManager : public AInfo {
  GENERATED_BODY()

public:
  ALevelManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Level Manager")
  const class UTagsComponent* PlayerTags;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  const class AStoreExpansionManager* StoreExpansionManager;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  const class AStorePhaseManager* StorePhaseManager;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  const class ADayManager* DayManager;

  UPROPERTY()
  class ASpgHUD* HUD;  // * To show loading screens.

  UPROPERTY(EditAnywhere, Category = "Level Manager")
  TMap<ELevel, FName> LevelNames;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  TMap<FName, FName> StoreExpansionLevelMap;  // * LevelID -> LevelName

  UPROPERTY(EditAnywhere)
  class APlayerCommand* PlayerCommand;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  class ACutsceneManager* CutsceneManager;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  class AStore* Store;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  class AMarketLevel* MarketLevel;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  class AChurchLevel* ChurchLevel;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  class AStorePhaseLightingManager* StorePhaseLightingManager;

  UPROPERTY(EditAnywhere, Category = "Level Manager")
  ELevel CurrentLevel;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  ELevel LoadedLevel;

  void InitLoadStore(std::function<void()> _LevelReadyFunc);

  void BeginLoadLevel(ELevel Level, std::function<void()> _LevelReadyFunc = nullptr);
  void BeginUnloadLevel(ELevel Level, std::function<void()> _LevelUnloadedFunc = nullptr);

  UFUNCTION()
  void OnLevelShown();
  UFUNCTION()
  void OnLevelAddedToWorld(ULevel* InLevel, UWorld* InWorld);
  UFUNCTION()
  void OnLevelUnloaded();

  void InitLevel(ELevel Level);
  void EnterLevel(ELevel Level);
  void SaveLevelState(ELevel Level);

  void ExpandStoreSwitchLevel();

  // * Horrible but needed when loading a save while in a level.
  void ReloadCurrentLevel(std::function<void()> _LevelReadyFunc);
  UFUNCTION()
  void OnLevelUnloadedReload();
  UFUNCTION()
  void OnLevelShownReload();

  std::function<void()> LevelUnloadedFunc;
  std::function<void()> LevelReadyFunc;
};