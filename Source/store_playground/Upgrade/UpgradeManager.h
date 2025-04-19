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

  UPROPERTY(EditAnywhere)
  class ACustomerAIManager* CustomerAIManager;
  UPROPERTY(EditAnywhere)
  class AMarket* Market;
  UPROPERTY(EditAnywhere)
  class AGlobalDataManager* GlobalDataManager;
  UPROPERTY(EditAnywhere)
  class AGlobalStaticDataManager* GlobalStaticDataManager;
  UPROPERTY(EditAnywhere)
  class AAbilityManager* AbilityManager;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FName> SelectedUpgradeIDs;
  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FName> ActiveEffectIDs;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FName> UnlockedEconEventAbilityIDs;
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<FName, int32> EconEventAbilityCooldowns;
  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FEconEventAbility> ActiveEconEventAbilities;

  auto GetAvailableUpgrades(EUpgradeClass UpgradeClass) const -> TArray<FUpgrade>;
  auto GetSelectedUpgrades(EUpgradeClass UpgradeClass) const -> TArray<FUpgrade>;
  void SelectUpgrade(const FName UpgradeId);

  auto GetAvailableEconEventAbilities() const -> TArray<FEconEventAbility>;
  void ActivateEconEventAbility(const FName AbilityId);

  void TickDaysTimedVars();

  UPROPERTY(EditAnywhere)
  FUpgradeable Upgradeable;
  void UnlockIDs(const FName DataName, const TArray<FName>& Ids);
};