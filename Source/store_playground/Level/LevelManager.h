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
  const class ADayManager* DayManager;

  UPROPERTY()
  class ASpgHUD* HUD;  // * To show loading screens.

  UPROPERTY(EditAnywhere, Category = "Level Manager")
  TMap<ELevel, FName> LevelNames;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  TMap<EStoreExpansionLevel, FName> StoreExpansionLevelNames;

  UPROPERTY(EditAnywhere, Category = "Level Manager")
  class ACutsceneManager* CutsceneManager;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  class AStore* Store;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  class AMarketLevel* MarketLevel;

  UPROPERTY(EditAnywhere, Category = "Level Manager")
  ELevel CurrentLevel;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  ELevel LoadedLevel;

  void InitLoadStore(std::function<void()> _LevelReadyFunc);

  void BeginLoadLevel(ELevel Level, std::function<void()> _LevelReadyFunc = nullptr);
  void BeginUnloadLevel(ELevel Level);

  UFUNCTION()
  void OnLevelShown();

  void InitLevel(ELevel Level);
  void EnterLevel(ELevel Level);
  void SaveLevelState(ELevel Level);

  void ExpandStoreSwitchLevel(std::function<void()> _LevelReadyFunc = nullptr);

  // * Horrible but needed when loading a save while in a level.
  void ReloadCurrentLevel(std::function<void()> _LevelReadyFunc);
  UFUNCTION()
  void OnLevelUnloaded();
  UFUNCTION()
  void OnLevelShownReload();

  std::function<void()> LevelReadyFunc;  // * Callback for the player.
};