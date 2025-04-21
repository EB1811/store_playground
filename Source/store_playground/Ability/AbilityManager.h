// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "AbilityManager.generated.h"

USTRUCT()
struct FAbilityManagerParams {
  GENERATED_BODY()
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AAbilityManager : public AInfo {
  GENERATED_BODY()

public:
  AAbilityManager() {
    PrimaryActorTick.bCanEverTick = false;

    Upgradeable.UnlockIDs = [this](const FName DataName, const TArray<FName>& Ids) { UnlockIDs(DataName, Ids); };
  }

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  const class AGlobalDataManager* GlobalDataManager;

  UPROPERTY(EditAnywhere)
  class AStore* Store;
  UPROPERTY(EditAnywhere)
  class AMarket* Market;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FName> UnlockedEconEventAbilityIDs;
  UPROPERTY(EditAnywhere, SaveGame)
  TMap<FName, int32> EconEventAbilityCooldowns;
  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FEconEventAbility> ActiveEconEventAbilities;

  auto GetAvailableEconEventAbilities() const -> TArray<FEconEventAbility>;
  void ActivateEconEventAbility(const FName AbilityId);

  void TickDaysTimedVars();

  UPROPERTY(EditAnywhere)
  FUpgradeable Upgradeable;
  void UnlockIDs(const FName DataName, const TArray<FName>& Ids);
};