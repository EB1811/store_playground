#include "UpgradeManager.h"
#include "Containers/Map.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Market/Market.h"

AUpgradeManager::AUpgradeManager() {
  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickInterval = 1.0f;
}

void AUpgradeManager::BeginPlay() {
  Super::BeginPlay();

  InitializeUpgradesData();
}

void AUpgradeManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

TArray<FUpgrade> AUpgradeManager::GetAvailableUpgrades(EUpgradeClass UpgradeClass) {
  return Upgrades.FilterByPredicate([this, UpgradeClass](const FUpgrade& Upgrade) {
    return Upgrade.UpgradeClass == UpgradeClass &&
           !SelectedUpgrades.ContainsByPredicate(
               [Upgrade](const FUpgrade& SelectedUpgrade) { return SelectedUpgrade.ID == Upgrade.ID; });
  });
}

TArray<FUpgrade> AUpgradeManager::GetSelectedUpgrades(EUpgradeClass UpgradeClass) {
  return SelectedUpgrades.FilterByPredicate(
      [UpgradeClass](const FUpgrade& Upgrade) { return Upgrade.UpgradeClass == UpgradeClass; });
}

void AUpgradeManager::SelectUpgrade(const FName UpgradeId) {
  const TMap<EUpgradeEffectSystem, FUpgradeable> UpgradeableMap = {
      {{EUpgradeEffectSystem::Market, Market->Upgradeable}},
  };

  FUpgrade* Upgrade =
      Upgrades.FindByPredicate([UpgradeId](const FUpgrade& Upgrade) { return Upgrade.ID == UpgradeId; });
  check(Upgrade);

  TArray<FUpgradeEffect> Effects = UpgradeEffects.FilterByPredicate(
      [Upgrade](const FUpgradeEffect& Effect) { return Upgrade->UpgradeEffectIDs.Contains(Effect.ID); });
  for (const FUpgradeEffect& Effect : Effects) {
    FUpgradeable Upgradeable = UpgradeableMap[Effect.EffectSystem];

    switch (Effect.EffectType) {
      case EUpgradeEffectType::ChangeBehaviorParam: {
        checkf(Upgradeable.ChangeBehaviorParam, TEXT("Upgradeable does not implement ChangeBehaviorParam."));
        check(Effect.RelevantValues.Num() > 0);

        Upgradeable.ChangeBehaviorParam(Effect.RelevantValues);
        break;
      }
      case EUpgradeEffectType::ChangeDataParam: {
        checkf(Upgradeable.ChangeDataParam, TEXT("Upgradeable does not implement ChangeDataParam."));
        check(Effect.RelevantValues.Num() > 0);

        Upgradeable.ChangeDataParam(Effect.RelevantValues);
        break;
      }
      case EUpgradeEffectType::UnlockIDs: {
        checkf(Upgradeable.UnlockIDs, TEXT("Upgradeable does not implement UnlockIDs."));
        check(!Effect.RelevantName.IsNone() && Effect.RelevantIDs.Num() > 0);

        Upgradeable.UnlockIDs(Effect.RelevantName, Effect.RelevantIDs);
        break;
      }
      case EUpgradeEffectType::FeatureUnlock: {
        checkf(Upgradeable.FeatureUnlock, TEXT("Upgradeable does not implement FeatureUnlock."));
        check(!Effect.RelevantName.IsNone());

        Upgradeable.FeatureUnlock(Effect.RelevantName);
        break;
      }
      case EUpgradeEffectType::Function: {
        checkf(Upgradeable.Function, TEXT("Upgradeable does not implement Function."));
        check(!Effect.RelevantName.IsNone());

        Upgradeable.Function(Effect.RelevantName);
        break;
      }
      default: checkNoEntry();
    }
  }
}

void AUpgradeManager::InitializeUpgradesData() {
  check(UpgradeEffectsTable && UpgradesTable);

  UpgradeEffects.Empty();
  TArray<FUpgradeEffectRow*> UpgradeEffectsRows;
  UpgradeEffectsTable->GetAllRows<FUpgradeEffectRow>("", UpgradeEffectsRows);
  for (auto Row : UpgradeEffectsRows)
    UpgradeEffects.Add({
        Row->ID,
        Row->EffectType,
        Row->EffectSystem,
        Row->RelevantName,
        Row->RelevantIDs,
        Row->RelevantValues,
        Row->TextData,
        Row->AssetData,
    });

  Upgrades.Empty();
  TArray<FUpgradeRow*> UpgradesRows;
  UpgradesTable->GetAllRows<FUpgradeRow>("", UpgradesRows);
  for (auto Row : UpgradesRows)
    Upgrades.Add({
        Row->ID,
        Row->UpgradeClass,
        Row->UpgradeEffectIDs,
        Row->TextData,
        Row->AssetData,
    });

  check(UpgradeEffects.Num() > 0);
  check(Upgrades.Num() > 0);

  UpgradeEffectsTable = nullptr;
  UpgradesTable = nullptr;
}