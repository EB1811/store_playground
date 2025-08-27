#include "UpgradeManager.h"
#include "Containers/Map.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Ability/AbilityManager.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "store_playground/UI/SpgHUD.h"

AUpgradeManager::AUpgradeManager() {
  PrimaryActorTick.bCanEverTick = false;

  Upgradeable.UnlockIDs = [this](const FName DataName, const TArray<FName>& Ids) { UnlockIDs(DataName, Ids); };
}

void AUpgradeManager::BeginPlay() {
  Super::BeginPlay();

  check(UpgradePointsGenDataTable);

  TArray<FUpgradePointsGenRow*> UpgradePointsGenRows;
  UpgradePointsGenDataTable->GetAllRows<FUpgradePointsGenRow>("", UpgradePointsGenRows);
  for (auto Row : UpgradePointsGenRows) {
    AllUpgradePointsGenArray.Add({
        Row->ID,
        Row->Requirements,
        Row->PointsGained,
        false,
    });
  }
  check(AllUpgradePointsGenArray.Num() > 0);

  UpgradePointsGenDataTable = nullptr;
}

void AUpgradeManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AUpgradeManager::ConsiderUpgradePoints() {
  const auto& GameDataMap = GlobalDataManager->GetGameDataMap();
  auto Filtered = AllUpgradePointsGenArray.FilterByPredicate([this, GameDataMap](const auto& Gen) {
    return !UpgradePointsGenArrayUsed.ContainsByPredicate([&](const auto& UsedGen) { return UsedGen.ID == Gen.ID; }) &&
           EvaluateRequirementsFilter(Gen.Requirements, GameDataMap);
  });
  if (Filtered.Num() == 0) return;

  for (auto& Gen : Filtered) {
    GainUpgradePoints(Gen.PointsGained);
    auto* UsedGen =
        AllUpgradePointsGenArray.FindByPredicate([&](const auto& ArrayGen) { return ArrayGen.ID == Gen.ID; });
    UpgradePointsGenArrayUsed.Add({
        UsedGen->ID,
        UsedGen->Requirements,
        UsedGen->PointsGained,
        true,
    });
  }
}
void AUpgradeManager::GainUpgradePoints(int32 Points) {
  AvailableUpgradePoints += Points;

  HUD->NotifyUpgradePointsGained();
}

auto AUpgradeManager::GetUpgradeEffectsByIds(const TArray<FName>& EffectIDs) const -> TArray<FUpgradeEffect> {
  return GlobalStaticDataManager->GetUpgradeEffectsByIds(EffectIDs);
}

auto AUpgradeManager::GetAvailableUpgrades(EUpgradeClass UpgradeClass) const -> TArray<FUpgrade> {
  int32 Level = 1;
  for (const auto& Pair : UpgradeManagerParams.UpgradeLevelRevenueReq)
    if (StatisticsGen->StoreStatistics.TotalRevenueToDate >= Pair.Value) Level = Pair.Key;
    else break;

  return GlobalStaticDataManager->GetEligibleUpgrades(UpgradeClass, Level, SelectedUpgradeIDs);
}

auto AUpgradeManager::GetUpgradesReqsNotMet(EUpgradeClass UpgradeClass) const -> TArray<FUpgrade> {
  int32 Level = 1;
  for (const auto& Pair : UpgradeManagerParams.UpgradeLevelRevenueReq)
    if (StatisticsGen->StoreStatistics.TotalRevenueToDate >= Pair.Value) Level = Pair.Key;
    else break;

  return GlobalStaticDataManager->GetUpgradesReqsNotMet(UpgradeClass, Level, SelectedUpgradeIDs);
}

auto AUpgradeManager::GetSelectedUpgrades(EUpgradeClass UpgradeClass) const -> TArray<FUpgrade> {
  const auto& AllSelectedUpgrades = GlobalStaticDataManager->GetUpgradesByIds(SelectedUpgradeIDs);
  return AllSelectedUpgrades.FilterByPredicate(
      [&](const FUpgrade& Upgrade) { return Upgrade.UpgradeClass == UpgradeClass; });
}

void AUpgradeManager::SelectUpgrade(const FName UpgradeId) {
  if (AvailableUpgradePoints <= 0) return;

  UE_LOG(LogTemp, Log, TEXT("Selected upgrade: %s"), *UpgradeId.ToString());

  FUpgrade Upgrade = GlobalStaticDataManager->GetUpgradeById(UpgradeId);
  check(Upgrade.Cost <= AvailableUpgradePoints);

  AvailableUpgradePoints -= Upgrade.Cost;

  TArray<FUpgradeEffect> Effects = GlobalStaticDataManager->GetUpgradeEffectsByIds(Upgrade.UpgradeEffectIDs);
  for (const FUpgradeEffect& Effect : Effects) {
    FUpgradeable SystemUpgradeable = UpgradeableMap[Effect.EffectSystem];

    switch (Effect.EffectType) {
      case EUpgradeEffectType::ChangeBehaviorParam: {
        checkf(SystemUpgradeable.ChangeBehaviorParam, TEXT("Upgradeable does not implement ChangeBehaviorParam."));
        check(Effect.RelevantValues.Num() > 0);

        SystemUpgradeable.ChangeBehaviorParam(Effect.RelevantValues);
        break;
      }
      case EUpgradeEffectType::ChangeData: {
        checkf(SystemUpgradeable.ChangeData, TEXT("Upgradeable does not implement ChangeData."));
        check(!Effect.RelevantName.IsNone() && Effect.RelevantValues.Num() > 0);

        SystemUpgradeable.ChangeData(Effect.RelevantName, Effect.RelevantIDs, Effect.RelevantValues);
        break;
      }
      case EUpgradeEffectType::UnlockIDs: {
        checkf(SystemUpgradeable.UnlockIDs, TEXT("Upgradeable does not implement UnlockIDs."));
        check(!Effect.RelevantName.IsNone() && Effect.RelevantIDs.Num() > 0);

        SystemUpgradeable.UnlockIDs(Effect.RelevantName, Effect.RelevantIDs);
        break;
      }
      case EUpgradeEffectType::FeatureUnlock: {
        checkf(SystemUpgradeable.FeatureUnlock, TEXT("Upgradeable does not implement FeatureUnlock."));
        check(!Effect.RelevantName.IsNone());

        SystemUpgradeable.FeatureUnlock(Effect.RelevantName);
        break;
      }
      case EUpgradeEffectType::UpgradeFunction: {
        checkf(SystemUpgradeable.UpgradeFunction, TEXT("Upgradeable does not implement UpgradeFunction."));
        check(!Effect.RelevantName.IsNone());

        SystemUpgradeable.UpgradeFunction(Effect.RelevantName, Effect.RelevantIDs, Effect.RelevantValues);
        break;
      }
      default: checkNoEntry();
    }

    ActiveEffectIDs.Add(Effect.ID);
  }

  SelectedUpgradeIDs.Add(UpgradeId);
}

void AUpgradeManager::TickDaysTimedVars() {}

void AUpgradeManager::UnlockIDs(const FName DataName, const TArray<FName>& Ids) {}