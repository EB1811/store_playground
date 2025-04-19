#include "AbilityManager.h"
#include "Containers/Map.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Framework/GlobalDataManager.h"

void AAbilityManager::BeginPlay() { Super::BeginPlay(); }

void AAbilityManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

auto AAbilityManager::GetAvailableEconEventAbilities() const -> TArray<FEconEventAbility> {
  const auto& AvailableEconEventAbilities = GlobalDataManager->GetEconEventAbilitiesByIds(UnlockedEconEventAbilityIDs);
  return AvailableEconEventAbilities.FilterByPredicate([&](const FEconEventAbility& Ability) {
    return !EconEventAbilityCooldowns.Contains(Ability.ID) &&
           !ActiveEconEventAbilities.ContainsByPredicate(
               [&](const FEconEventAbility& ActiveAbility) { return ActiveAbility.ID == Ability.ID; });
  });
}

void AAbilityManager::ActivateEconEventAbility(const FName AbilityId) {
  check(!EconEventAbilityCooldowns.Contains(AbilityId));
  check(!ActiveEconEventAbilities.ContainsByPredicate(
      [&](const FEconEventAbility& ActiveAbility) { return ActiveAbility.ID == AbilityId; }));

  FEconEventAbility Ability = GlobalDataManager->GetEconEventAbilitiesByIds({AbilityId})[0];
  check(!Ability.EconEventId.IsNone());

  Market->AddGuaranteedEconEvents({Ability.EconEventId});
  ActiveEconEventAbilities.Add(Ability);
}

void AAbilityManager::TickDaysTimedVars() {
  TArray<FEconEventAbility> ActiveAbilitiesToRemove;
  TMap<FName, int32> CooldownsToAdd;
  for (auto& EconEventAbility : ActiveEconEventAbilities)
    if (EconEventAbility.DurationLeft <= 1) ActiveAbilitiesToRemove.Add(EconEventAbility);
    else EconEventAbility.DurationLeft -= 1;
  for (const auto& EconEventAbility : ActiveAbilitiesToRemove) {
    ActiveEconEventAbilities.RemoveAllSwap(
        [&](const FEconEventAbility& Ability) { return Ability.ID == EconEventAbility.ID; });

    if (EconEventAbility.Cooldown > 0) CooldownsToAdd.Add(EconEventAbility.ID, EconEventAbility.Cooldown);
  }

  TArray<FName> CooldownsToRemove;
  for (auto& Cooldown : EconEventAbilityCooldowns)
    if (Cooldown.Value <= 1) CooldownsToRemove.Add(Cooldown.Key);
    else Cooldown.Value -= 1;
  for (const auto& CooldownId : CooldownsToRemove) EconEventAbilityCooldowns.Remove(CooldownId);

  for (const auto& Cooldown : CooldownsToAdd)
    if (!EconEventAbilityCooldowns.Contains(Cooldown.Key)) EconEventAbilityCooldowns.Add(Cooldown.Key, Cooldown.Value);
}

void AAbilityManager::UnlockIDs(const FName DataName, const TArray<FName>& Ids) {
  if (DataName != "EconEventAbility") checkf(false, TEXT("UnlockIDs only supports EconEventAbility IDs."));

  for (const auto& Id : Ids) {
    if (UnlockedEconEventAbilityIDs.Contains(Id)) continue;
    UnlockedEconEventAbilityIDs.Add(Id);
  }
}