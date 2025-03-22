#pragma once

#include "GameFramework/Actor.h"
#include "store_playground/Level/LevelManager.h"
#include "LevelStreamerActor.generated.h"

// * Responsible for loading and unloading levels dynamically using an overlap.
// ? Used only for higer settings? Lower settings would use load level upon button press.

UCLASS()
class STORE_PLAYGROUND_API ALevelStreamerActor : public AActor {
  GENERATED_BODY()

public:
  ALevelStreamerActor();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  class UBoxComponent* OverlapVolume;

  UPROPERTY(EditAnywhere)
  ELevel LevelToLoad;
  UPROPERTY(EditAnywhere)
  ELevel LevelToUnload;

  UPROPERTY(EditAnywhere)
  class TSubclassOf<class AActor> LevelManagerClass;

  UFUNCTION()
  void OverlapBegins(UPrimitiveComponent* OverlappedComponent,
                     AActor* OtherActor,
                     UPrimitiveComponent* OtherComp,
                     int32 OtherBodyIndex,
                     bool bFromSweep,
                     const FHitResult& SweepResult);
  UFUNCTION()
  void OverlapEnds(UPrimitiveComponent* OverlappedComponent,
                   AActor* OtherActor,
                   UPrimitiveComponent* OtherComp,
                   int32 OtherBodyIndex);
};