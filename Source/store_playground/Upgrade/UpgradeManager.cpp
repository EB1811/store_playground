#include "UpgradeManager.h"
#include "Containers/Map.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Ability/AbilityManager.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"

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
    UpgradePointsGenArray.Add({
        Row->ID,
        Row->Requirements,
        Row->PointsGained,
        false,
    });
  }
  check(UpgradePointsGenArray.Num() > 0);

  UpgradePointsGenDataTable = nullptr;
}

void AUpgradeManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AUpgradeManager::ConsiderUpgradePoints() {
  const auto& GameDataMap = GlobalDataManager->GetGameDataMap();
  auto Filtered = UpgradePointsGenArray.FilterByPredicate(
      [&](const auto& Gen) { return !Gen.bHasBeenUsed && EvaluateRequirementsFilter(Gen.Requirements, GameDataMap); });
  if (Filtered.Num() == 0) return;

  for (auto& Gen : Filtered) {
    GainUpgradePoints(Gen.PointsGained);
    auto* UsedGen = UpgradePointsGenArray.FindByPredicate([&](const auto& ArrayGen) { return ArrayGen.ID == Gen.ID; });
    UsedGen->bHasBeenUsed = true;
  }
}
void AUpgradeManager::GainUpgradePoints(int32 Points) { AvailableUpgradePoints += Points; }

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

  const TMap<EUpgradeEffectSystem, FUpgradeable> UpgradeableMap = {
      {EUpgradeEffectSystem::Ability, AbilityManager->Upgradeable},
      {EUpgradeEffectSystem::CustomerAI, CustomerAIManager->Upgradeable},
      {EUpgradeEffectSystem::Market, Market->Upgradeable},
      {EUpgradeEffectSystem::GlobalData, GlobalDataManager->Upgradeable},
  };
  UE_LOG(LogTemp, Warning, TEXT("Selected upgrade: %s"), *UpgradeId.ToString());

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

auto AUpgradeManager::GetAvailableEconEventAbilities() const -> TArray<FEconEventAbility> {
  const auto& AvailableEconEventAbilities = GlobalDataManager->GetEconEventAbilitiesByIds(UnlockedEconEventAbilityIDs);
  return AvailableEconEventAbilities.FilterByPredicate([&](const FEconEventAbility& Ability) {
    return !EconEventAbilityCooldowns.Contains(Ability.ID) &&
           !ActiveEconEventAbilities.ContainsByPredicate(
               [&](const FEconEventAbility& ActiveAbility) { return ActiveAbility.ID == Ability.ID; });
  });
}

void AUpgradeManager::ActivateEconEventAbility(const FName AbilityId) {
  check(!EconEventAbilityCooldowns.Contains(AbilityId));
  check(!ActiveEconEventAbilities.ContainsByPredicate(
      [&](const FEconEventAbility& ActiveAbility) { return ActiveAbility.ID == AbilityId; }));

  FEconEventAbility Ability = GlobalDataManager->GetEconEventAbilitiesByIds({AbilityId})[0];
  check(!Ability.EconEventId.IsNone());

  Market->AddGuaranteedEconEvents({Ability.EconEventId});
  ActiveEconEventAbilities.Add(Ability);
}

void AUpgradeManager::TickDaysTimedVars() {
  TArray<FEconEventAbility> EconEventAbilitiesToRemove;
  for (auto& EconEventAbility : ActiveEconEventAbilities)
    if (EconEventAbility.DurationLeft <= 1) EconEventAbilitiesToRemove.Add(EconEventAbility);
    else EconEventAbility.DurationLeft -= 1;
  for (const auto& EconEventAbility : EconEventAbilitiesToRemove) {
    ActiveEconEventAbilities.RemoveAllSwap(
        [&](const FEconEventAbility& Ability) { return Ability.ID == EconEventAbility.ID; });

    EconEventAbilityCooldowns.Add(EconEventAbility.ID, EconEventAbility.Cooldown);
  }

  TArray<FName> EconEventAbilityCooldownsToRemove;
  for (auto& Cooldown : EconEventAbilityCooldowns)
    if (Cooldown.Value <= 1) EconEventAbilityCooldownsToRemove.Add(Cooldown.Key);
    else Cooldown.Value -= 1;
  for (const auto& CooldownId : EconEventAbilityCooldownsToRemove) EconEventAbilityCooldowns.Remove(CooldownId);
}

void AUpgradeManager::UnlockIDs(const FName DataName, const TArray<FName>& Ids) {
  if (DataName != "EconEventAbility") checkf(false, TEXT("UnlockIDs only supports EconEventAbility IDs."));

  for (const auto& Id : Ids) {
    if (UnlockedEconEventAbilityIDs.Contains(Id)) continue;
    UnlockedEconEventAbilityIDs.Add(Id);
  }
}