// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "UpgradeManager.generated.h"

USTRUCT()
struct FUpgradeManagerParams {
  GENERATED_BODY()
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AUpgradeManager : public AInfo {
  GENERATED_BODY()

public:
  AUpgradeManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Upgrades")
  TObjectPtr<const class UDataTable> UpgradeEffectsTable;
  UPROPERTY(EditAnywhere, Category = "Upgrades")
  TObjectPtr<const class UDataTable> UpgradesTable;

  UPROPERTY(EditAnywhere, Category = "Upgrades")
  TArray<FUpgradeEffect> UpgradeEffects;
  UPROPERTY(EditAnywhere, Category = "Upgrades")
  TArray<FUpgrade> Upgrades;

  UPROPERTY(EditAnywhere, Category = "Upgrades")
  class ACustomerAIManager* CustomerAIManager;
  UPROPERTY(EditAnywhere, Category = "Upgrades")
  class AMarket* Market;
  UPROPERTY(EditAnywhere, Category = "Upgrades")
  class AGlobalDataManager* GlobalDataManager;

  UPROPERTY(EditAnywhere)
  TArray<FUpgrade> SelectedUpgrades;
  UPROPERTY(EditAnywhere)
  TArray<FUpgradeEffect> ActiveEffects;

  auto GetAvailableUpgrades(EUpgradeClass UpgradeClass) const -> TArray<FUpgrade>;
  auto GetSelectedUpgrades(EUpgradeClass UpgradeClass) const -> TArray<FUpgrade>;

  void SelectUpgrade(const FName UpgradeId);

  void InitializeUpgradesData();
};