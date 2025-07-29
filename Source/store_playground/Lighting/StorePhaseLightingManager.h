// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "StorePhaseLightingManager.generated.h"

USTRUCT()
struct FStorePhaseLightingManagerParams {
  GENERATED_BODY()
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AStorePhaseLightingManager : public AInfo {
  GENERATED_BODY()

public:
  AStorePhaseLightingManager() { PrimaryActorTick.bCanEverTick = false; }

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Lighting")
  FStorePhaseLightingManagerParams StorePhaseLightingManagerParams;

  // * Lighting changes done in blueprints for fine tuning iterations.
  UFUNCTION(BlueprintImplementableEvent)
  void OnEndDayLightingCalled();
  UFUNCTION(BlueprintImplementableEvent)
  void OnOpenShopLightingCalled();
  UFUNCTION(BlueprintImplementableEvent)
  void OnCloseShopLightingCalled();

  UFUNCTION(BlueprintImplementableEvent)
  void SetupMarketLevelLighting();
  UFUNCTION(BlueprintImplementableEvent)
  void SetupStoreLevelDayLighting();
  UFUNCTION(BlueprintImplementableEvent)
  void SetupStoreLevelNightLighting();
  UFUNCTION(BlueprintImplementableEvent)
  void SetupChurchLevelNightLighting();
};