#include "DayManager.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/Framework/StorePGGameInstance.h"
#include "store_playground/Framework/StorePGGameMode.h"
#include "store_playground/Framework/SettingsManager.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Level/MarketLevel.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Ability/AbilityManager.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "store_playground/Steam/SteamManager.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "Kismet/GameplayStatics.h"

ADayManager::ADayManager() {
  PrimaryActorTick.bCanEverTick = false;

  CurrentDay = 0;
  NextDayToPayDebt = -1;
  NextDebtAmount = 0.0f;
}

void ADayManager::BeginPlay() {
  Super::BeginPlay();

  for (auto Type : TEnumRange<EGameDifficulty>()) check(DayManagerParams.DifficultyDebtMultiMap.Contains(Type));
}

void ADayManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ADayManager::StartNewDay() {
  check(Market && MarketLevel && NewsGen);
  UE_LOG(LogTemp, Warning, TEXT("DayManager: Starting new day."));

  if (CheckDemoVersion()) return;

  // Manage debt first.
  if (ManageDebt()) return;

  CurrentDay++;
  bIsWeekend = CurrentDay % DayManagerParams.WeekendDivisor == 0      ? true
               : CurrentDay == 1 && DayManagerParams.bDayOneIsWeekend ? true
                                                                      : false;

  AbilityManager->TickDaysTimedVars();
  CustomerAIManager->TickDaysTimedVars();
  MarketEconomy->TickDaysActivePriceEffects();
  Market->TickDaysTimedVars();
  MarketLevel->TickDaysTimedVars();
  NewsGen->TickDaysTimedVars();

  StatisticsGen->CalcDayStatistics();

  Market->ConsiderEconEvents();
  NewsGen->GenDaysRandomArticles();

  MarketLevel->ResetDaysLevelState();

  SteamManager->ConsiderAchievements();
}

auto ADayManager::ManageDebt() -> bool {
  if (CurrentDay < NextDayToPayDebt) return false;
  if (CurrentDay > NextDayToPayDebt) {
    NextDayToPayDebt = CurrentDay + DayManagerParams.DebtPaymentDayDivisor;
    NextDebtAmount = DayManagerParams.BaseDebtAmount *
                     (DayManagerParams.DebtIncreaseMulti *
                      FMath::Floor(float(CurrentDay) / float(DayManagerParams.DebtPaymentDayDivisor) + 1.0f));

    AStorePGGameMode* GameMode = Cast<AStorePGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    check(GameMode && GameMode->SettingsManager);
    EGameDifficulty Difficulty = GameMode->SettingsManager->GameSettings.Difficulty;
    NextDebtAmount *= DayManagerParams.DifficultyDebtMultiMap[Difficulty];

    return false;
  }

  if (Store->GetAvailableMoney() >= NextDebtAmount) {
    Store->MoneySpent(NextDebtAmount);

    NextDayToPayDebt = CurrentDay + DayManagerParams.DebtPaymentDayDivisor;
    NextDebtAmount = DayManagerParams.BaseDebtAmount *
                     (DayManagerParams.DebtIncreaseMulti *
                      FMath::Floor(float(CurrentDay) / float(DayManagerParams.DebtPaymentDayDivisor) + 1.0f));

    AStorePGGameMode* GameMode = Cast<AStorePGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    check(GameMode && GameMode->SettingsManager);
    EGameDifficulty Difficulty = GameMode->SettingsManager->GameSettings.Difficulty;
    NextDebtAmount *= DayManagerParams.DifficultyDebtMultiMap[Difficulty];

    return false;
  }

  // Game over.
  UE_LOG(LogTemp, Warning, TEXT("DayManager: Game over. Not enough money to pay debt."));
  AStorePGGameMode* GameMode = Cast<AStorePGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  check(GameMode);
  GameMode->GameOverReset();
  return true;
}
void ADayManager::RecalculateNextDebt() {
  NextDebtAmount = DayManagerParams.BaseDebtAmount *
                   (DayManagerParams.DebtIncreaseMulti *
                    FMath::Floor(float(CurrentDay - 1) / float(DayManagerParams.DebtPaymentDayDivisor) + 1.0f));

  AStorePGGameMode* GameMode = Cast<AStorePGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  check(GameMode && GameMode->SettingsManager);
  EGameDifficulty Difficulty = GameMode->SettingsManager->GameSettings.Difficulty;
  NextDebtAmount *= DayManagerParams.DifficultyDebtMultiMap[Difficulty];
}

auto ADayManager::CheckDemoVersion() -> bool {
  UStorePGGameInstance* StorePGGameInstance = Cast<UStorePGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
  check(StorePGGameInstance);
  if (!StorePGGameInstance->bIsDemoVersion) return false;

  if (CurrentDay >= DayManagerParams.DemoVersionMaxDays) {
    UE_LOG(LogTemp, Warning, TEXT("DayManager: Demo version max days reached. Ending game."));
    AStorePGGameMode* GameMode = Cast<AStorePGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    check(GameMode);
    GameMode->DemoGameOver();
    return true;
  }
  return false;
}