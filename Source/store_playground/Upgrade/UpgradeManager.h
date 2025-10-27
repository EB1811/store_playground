// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "UpgradeManager.generated.h"

USTRUCT()
struct FUpgradeManagerParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  TMap<int32, int32> UpgradeLevelSelectedReq;  // Level to total selected upgrades required.
  UPROPERTY(EditAnywhere)
  int32 MaxUpgradePoints;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AUpgradeManager : public AInfo {
  GENERATED_BODY()

public:
  AUpgradeManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  FUpgradeManagerParams UpgradeManagerParams;

  UPROPERTY(EditAnywhere)
  TObjectPtr<const class UDataTable> UpgradePointsGenDataTable;
  UPROPERTY(EditAnywhere)
  TArray<FUpgradePointsGen> AllUpgradePointsGenArray;

  UPROPERTY(EditAnywhere)
  const class AStatisticsGen* StatisticsGen;

  UPROPERTY(EditAnywhere)
  class ASpgHUD* HUD;
  UPROPERTY(EditAnywhere)
  class AGlobalDataManager* GlobalDataManager;
  UPROPERTY(EditAnywhere)
  class AGlobalStaticDataManager* GlobalStaticDataManager;

  UPROPERTY(EditAnywhere)
  TMap<EUpgradeEffectSystem, FUpgradeable> UpgradeableMap;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FUpgradePointsGen> UpgradePointsGenArrayUsed;

  UPROPERTY(EditAnywhere, SaveGame)
  int32 AvailableUpgradePoints;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FName> SelectedUpgradeIDs;
  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FName> ActiveEffectIDs;

  void ConsiderUpgradePoints();  // ? Maybe tick this? Currently called by StatisticsGen on any change.
  void GainUpgradePoints(int32 Points);

  auto GetUpgradeEffectsByIds(const TArray<FName>& EffectIDs) const -> TArray<FUpgradeEffect>;
  auto GetAvailableUpgrades(EUpgradeClass UpgradeClass) const -> TArray<FUpgrade>;
  auto GetUpgradesReqsNotMet(EUpgradeClass UpgradeClass) const -> TArray<FUpgrade>;
  auto GetSelectedUpgrades(EUpgradeClass UpgradeClass) const -> TArray<FUpgrade>;

  void SelectUpgrade(const FName UpgradeId);

  void TickDaysTimedVars();

  UPROPERTY(EditAnywhere)
  FUpgradeable Upgradeable;
  void UnlockIDs(const FName DataName, const TArray<FName>& Ids);
};