#pragma once

#include "GameFramework/Info.h"
#include "LevelManager.generated.h"

// * Responsible for loading and unloading levels dynamically.
// ? Store variables on level unload to use later?

UENUM()
enum class ELevel : uint8 {
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
  ELevel LoadedLevel;

  UFUNCTION(BlueprintCallable, Category = "Level Manager")
  void LoadLevel(ELevel Level);
  UFUNCTION(BlueprintCallable, Category = "Level Manager")
  void UnloadLevel(ELevel Level);

  UFUNCTION(BlueprintCallable, Category = "Level Manager")
  void InitLevel();
};