#include "UpgradeManager.h"
#include "Containers/Map.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Framework/GlobalDataManager.h"

AUpgradeManager::AUpgradeManager() { PrimaryActorTick.bCanEverTick = false; }

void AUpgradeManager::BeginPlay() { Super::BeginPlay(); }

void AUpgradeManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

auto AUpgradeManager::GetAvailableUpgrades(EUpgradeClass UpgradeClass) const -> TArray<FUpgrade> {
  return GlobalDataManager->GetAvailableUpgrades(UpgradeClass, SelectedUpgradeIDs);
}

auto AUpgradeManager::GetSelectedUpgrades(EUpgradeClass UpgradeClass) const -> TArray<FUpgrade> {
  const auto& AllSelectedUpgrades = GlobalDataManager->GetUpgradesByIds(SelectedUpgradeIDs);
  return AllSelectedUpgrades.FilterByPredicate(
      [&](const FUpgrade& Upgrade) { return Upgrade.UpgradeClass == UpgradeClass; });
}

void AUpgradeManager::SelectUpgrade(const FName UpgradeId) {
  const TMap<EUpgradeEffectSystem, FUpgradeable> UpgradeableMap = {
      {EUpgradeEffectSystem::CustomerAI, CustomerAIManager->Upgradeable},
      {EUpgradeEffectSystem::Market, Market->Upgradeable},
      {EUpgradeEffectSystem::GlobalData, GlobalDataManager->Upgradeable},
  };

  UE_LOG(LogTemp, Warning, TEXT("Selected upgrade: %s"), *UpgradeId.ToString());

  FUpgrade Upgrade = GlobalDataManager->GetUpgradeById(UpgradeId);

  TArray<FUpgradeEffect> Effects = GlobalDataManager->GetUpgradeEffectsByIds(Upgrade.UpgradeEffectIDs);
  for (const FUpgradeEffect& Effect : Effects) {
    FUpgradeable Upgradeable = UpgradeableMap[Effect.EffectSystem];

    switch (Effect.EffectType) {
      case EUpgradeEffectType::ChangeBehaviorParam: {
        checkf(Upgradeable.ChangeBehaviorParam, TEXT("Upgradeable does not implement ChangeBehaviorParam."));
        check(Effect.RelevantValues.Num() > 0);

        Upgradeable.ChangeBehaviorParam(Effect.RelevantValues);
        break;
      }
      case EUpgradeEffectType::ChangeData: {
        checkf(Upgradeable.ChangeData, TEXT("Upgradeable does not implement ChangeData."));
        check(!Effect.RelevantName.IsNone() && Effect.RelevantValues.Num() > 0);

        Upgradeable.ChangeData(Effect.RelevantName, Effect.RelevantIDs, Effect.RelevantValues);
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
      case EUpgradeEffectType::UpgradeFunction: {
        checkf(Upgradeable.UpgradeFunction, TEXT("Upgradeable does not implement UpgradeFunction."));
        check(!Effect.RelevantName.IsNone());

        Upgradeable.UpgradeFunction(Effect.RelevantName, Effect.RelevantIDs, Effect.RelevantValues);
        break;
      }
      default: checkNoEntry();
    }

    ActiveEffectIDs.Add(Effect.ID);
  }

  SelectedUpgradeIDs.Add(UpgradeId);
}