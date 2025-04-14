#pragma once

#include <functional>
#include "GameFramework/Info.h"
#include "LevelManager.generated.h"

// * Responsible for loading and unloading levels dynamically.

UENUM()
enum class ELevel : uint8 {
  None UMETA(DisplayName = "None"),
  Store UMETA(DisplayName = "Store"),
  Market UMETA(DisplayName = "Market"),
  Church UMETA(DisplayName = "Church"),
};
ENUM_RANGE_BY_COUNT(ELevel, 4);

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ALevelManager : public AInfo {
  GENERATED_BODY()

public:
  ALevelManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Level Manager")
  TMap<ELevel, FName> LevelNames;

  UPROPERTY(EditAnywhere, Category = "Level Manager")
  class AStore* Store;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  class AMarket* Market;

  UPROPERTY(EditAnywhere, Category = "Level Manager")
  ELevel CurrentLevel;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  ELevel LoadedLevel;

  void LoadLevel(ELevel Level);  // Blocking true.

  void BeginLoadLevel(ELevel Level, std::function<void()> _LevelReadyFunc = nullptr);
  void BeginUnloadLevel(ELevel Level);

  UFUNCTION()
  void OnLevelShown();

  void InitLevel(ELevel Level);
  void SaveLevelState(ELevel Level);

  // * Horrible but needed when loading a save while in a level.
  void ReloadCurrentLevel(std::function<void()> _LevelReadyFunc);
  UFUNCTION()
  void OnLevelUnloaded();
  UFUNCTION()
  void OnLevelShownReload();

  std::function<void()> LevelReadyFunc;  // * Callback for the player.
};