#pragma once

#include <functional>
#include "GameFramework/Info.h"
#include "LevelManager.generated.h"

// * Responsible for loading and unloading levels dynamically.
// ? Store variables on level unload to use later?

UENUM()
enum class ELevel : uint8 {
  None UMETA(DisplayName = "None"),
  Store UMETA(DisplayName = "Store"),
  Market UMETA(DisplayName = "Market"),
  //
};
ENUM_RANGE_BY_COUNT(ELevel, 2);

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ALevelManager : public AInfo {
  GENERATED_BODY()

public:
  ALevelManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Level Manager")
  TMap<ELevel, FName> LevelNames;

  UPROPERTY(EditAnywhere)
  class AStore* Store;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  class AMarket* Market;

  UPROPERTY(EditAnywhere, Category = "Level Manager")
  ELevel CurrentLevel;
  UPROPERTY(EditAnywhere, Category = "Level Manager")
  ELevel LoadedLevel;

  void BeginLoadLevel(ELevel Level, std::function<void()> _LevelReadyFunc = nullptr);
  void BeginUnloadLevel(ELevel Level);

  UFUNCTION()
  void OnLevelShown();
  void InitLevel(ELevel Level);

  std::function<void()> LevelReadyFunc;  // * Callback for the player.
};