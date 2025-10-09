#include "SteamManager.h"
#include "Containers/Map.h"
#include "Logging/LogVerbosity.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Framework/GlobalDataManager.h"

void ASteamManager::BeginPlay() { Super::BeginPlay(); }

void ASteamManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ASteamManager::AwardAchievements(TArray<FSteamAchievement>& Achievements) {
  for (auto& Ach : Achievements) {
    auto* FoundAch =
        SteamAchievements.FindByPredicate([Ach](const auto& ArrayAch) { return ArrayAch.SteamID == Ach.SteamID; });
    check(FoundAch);
    FoundAch->bAchieved = true;

    if (SteamManagerParams.bEnableSteamIntegration) {
      // SteamAPI()->UserStats()->SetAchievement(TCHAR_TO_ANSI(*Ach.SteamID.ToString()));
      // SteamAPI()->UserStats()->StoreStats();
    }

    UE_LOG(LogTemp, Warning, TEXT("Awarded steam achievement %s"), *Ach.SteamID.ToString());
  }
}

void ASteamManager::ConsiderAchievements() {
  auto Filtered = SteamAchievements.FilterByPredicate(
      [](const auto& Ach) { return !Ach.bAchieved && Ach.Type == ESteamAchievementType::GameState; });
  if (Filtered.Num() <= 0) return;

  const auto& GameDataMap = GlobalDataManager->GetGameDataMap();

  auto AchsToAward = Filtered.FilterByPredicate(
      [GameDataMap](const auto& Ach) { return EvaluateRequirementsFilter(Ach.Requirements, GameDataMap); });
  AwardAchievements(AchsToAward);
}
void ASteamManager::AchBehaviour(FGameplayTag AchBehaviourTag) {
  auto Filtered = SteamAchievements.FilterByPredicate([AchBehaviourTag](const auto& Ach) {
    return !Ach.bAchieved &&
           (Ach.Type == ESteamAchievementType::Triggered || Ach.Type == ESteamAchievementType::Tracked) &&
           Ach.AchBehaviourTag.MatchesTag(AchBehaviourTag);
  });
  if (Filtered.Num() <= 0) return;

  const auto& GameDataMap = GlobalDataManager->GetGameDataMap();

  TArray<FSteamAchievement> AchsToAward;
  for (auto& Ach : Filtered) {
    if (Ach.Type == ESteamAchievementType::Tracked) {
      int32& CurrentCount = BehaviourTagCounts.FindOrAdd(AchBehaviourTag);
      CurrentCount++;

      if (CurrentCount >= Ach.TargetCount && EvaluateRequirementsFilter(Ach.Requirements, GameDataMap))
        AchsToAward.Add(Ach);
      continue;
    }

    if (EvaluateRequirementsFilter(Ach.Requirements, GameDataMap)) AchsToAward.Add(Ach);
  }
  AwardAchievements(AchsToAward);

  // Also consider game state achievements, as some triggered ones may unlock them.
  ConsiderAchievements();
}

void ASteamManager::AchItemDeal(const FItemDeal ItemDeal) {
  auto Filtered = SteamAchievements.FilterByPredicate(
      [ItemDeal](const auto& Ach) { return !Ach.bAchieved && Ach.Type == ESteamAchievementType::Custom; });
  if (Filtered.Num() <= 0) return;

  TArray<FSteamAchievement> AchsToAward;

  if (auto CustomAch = Filtered.FindByPredicate([](const auto& Ach) {
        return Ach.IdTag == FGameplayTag::RequestGameplayTag("Achievement.MerchantMaster");
      })) {
    if (ItemDeal.BoughtAt > 0.0f && ItemDeal.SoldAt > ItemDeal.BoughtAt * 2.0f) AchsToAward.Add(*CustomAch);
  }
  if (auto CustomAch = Filtered.FindByPredicate([](const auto& Ach) {
        return Ach.IdTag == FGameplayTag::RequestGameplayTag("Achievement.BuyHighSellLow");
      })) {
    if (ItemDeal.SoldAt < ItemDeal.BoughtAt - KINDA_SMALL_NUMBER) AchsToAward.Add(*CustomAch);
  }

  AwardAchievements(AchsToAward);
}