// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "StorePhaseManager.generated.h"

UENUM()
enum class EStorePhaseState : uint8 {
  None UMETA(DisplayName = "NONE"),
  Morning UMETA(DisplayName = "Morning"),
  MorningBuildMode UMETA(DisplayName = "Morning Build Mode"),  // * Toggle for when the player wants to build.
  ShopOpen UMETA(DisplayName = "Shop Open"),
  Night UMETA(DisplayName = "Night"),
};
UENUM()
enum class EStorePhaseAction : uint8 {
  Start UMETA(DisplayName = "Start"),
  OpenShop UMETA(DisplayName = "Open Shop"),
  ToggleBuildMode UMETA(DisplayName = "Toggle Build Mode"),
  CloseShop UMETA(DisplayName = "Close Shop"),
  EndDay UMETA(DisplayName = "End Day"),
};
EStorePhaseState GetNextStorePhaseState(EStorePhaseState CurrentState, EStorePhaseAction Action);

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AStorePhaseManager : public AInfo {
  GENERATED_BODY()

public:
  AStorePhaseManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Store")
  class TSubclassOf<class AActor> BuildableClass;

  UPROPERTY(EditAnywhere, Category = "Store Phase")
  EStorePhaseState StorePhaseState;

  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class ADayManager* DayManager;
  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class ACustomerAIManager* CustomerAIManager;

  void Start();
  void BuildMode();
  void OpenShop();
  void CloseShop();
  void EndDay();
  void NextPhase();
};