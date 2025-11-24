#include "SteamManager.h"
#include "Containers/Map.h"
#include "Logging/LogVerbosity.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "store_playground/Framework/StorePGGameInstance.h"
#include "Kismet/GameplayStatics.h"

void ASteamManager::BeginPlay() {
  Super::BeginPlay();

  QueryAchievements();
}

void ASteamManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ASteamManager::QueryAchievements() {
  if (!SteamManagerParams.bEnableSteamIntegration) return;

  IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
  if (!OnlineSub) {
    UE_LOG(LogTemp, Error, TEXT("Failed to get online subsystem"));
    return;
  }
  IOnlineIdentityPtr IdentityI = OnlineSub->GetIdentityInterface();
  IOnlineAchievementsPtr AchievementsI = OnlineSub->GetAchievementsInterface();
  if (!IdentityI.IsValid() || !AchievementsI.IsValid()) {
    UE_LOG(LogTemp, Error, TEXT("Failed to get online identity or achievements interface"));
    return;
  }
  FUniqueNetIdPtr UserId = IdentityI->GetUniquePlayerId(0);
  if (!UserId.IsValid()) {
    UE_LOG(LogTemp, Error, TEXT("Failed to get unique player id"));
    return;
  }

  AchievementsI->QueryAchievements(
      *UserId,
      FOnQueryAchievementsCompleteDelegate::CreateLambda([](const FUniqueNetId& InUserId, bool bWasSuccessful) {
        if (bWasSuccessful) {
          UE_LOG(LogTemp, Log, TEXT("Successfully queried achievements for user %s"), *InUserId.ToString());
          return;
        } else {
          UE_LOG(LogTemp, Error, TEXT("Failed to query achievements for user %s"), *InUserId.ToString());
          return;
        }
      }));
}

void ASteamManager::AwardAchievements(TArray<FSteamAchievement>& Achievements) {
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);
  if (StorePGGameInstance->bIsDemoVersion) return;

  if (!SteamManagerParams.bEnableSteamIntegration) {
    for (auto& Ach : Achievements) {
      auto* FoundAch =
          SteamAchievements.FindByPredicate([Ach](const auto& ArrayAch) { return ArrayAch.SteamID == Ach.SteamID; });
      check(FoundAch);
      FoundAch->bAchieved = true;

      UE_LOG(LogTemp, Warning, TEXT("Awarded steam achievement %s"), *Ach.SteamID.ToString());
    }
    return;
  }

  IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
  if (!OnlineSub) {
    UE_LOG(LogTemp, Error, TEXT("Failed to get online subsystem"));
    return;
  }
  IOnlineIdentityPtr IdentityI = OnlineSub->GetIdentityInterface();
  IOnlineAchievementsPtr AchievementsI = OnlineSub->GetAchievementsInterface();
  if (!IdentityI.IsValid() || !AchievementsI.IsValid()) {
    UE_LOG(LogTemp, Error, TEXT("Failed to get online identity or achievements interface"));
    return;
  }
  FUniqueNetIdPtr UserId = IdentityI->GetUniquePlayerId(0);
  if (!UserId.IsValid()) {
    UE_LOG(LogTemp, Error, TEXT("Failed to get unique player id"));
    return;
  }

  for (auto& Ach : Achievements) {
    auto* FoundAch =
        SteamAchievements.FindByPredicate([Ach](const auto& ArrayAch) { return ArrayAch.SteamID == Ach.SteamID; });
    check(FoundAch);
    FoundAch->bAchieved = true;

    FOnlineAchievementsWritePtr WriteObject = MakeShareable(new FOnlineAchievementsWrite());
    WriteObject->SetFloatStat(Ach.SteamID.ToString(), 100.0f);

    FOnlineAchievementsWriteRef WriteObjectRef = WriteObject.ToSharedRef();
    AchievementsI->WriteAchievements(
        *UserId, WriteObjectRef,
        FOnAchievementsWrittenDelegate::CreateLambda([Ach](const FUniqueNetId& InUserId, bool bWasSuccessful) {
          if (bWasSuccessful) {
            UE_LOG(LogTemp, Log, TEXT("Successfully wrote achievement %s for user %s"), *Ach.SteamID.ToString(),
                   *InUserId.ToString());
          } else {
            UE_LOG(LogTemp, Error, TEXT("Failed to write achievement %s for user %s"), *Ach.SteamID.ToString(),
                   *InUserId.ToString());
          }
        }));

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