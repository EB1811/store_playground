#include "DayManager.h"
#include "store_playground/Framework/StorePGGameMode.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Level/MarketLevel.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Ability/AbilityManager.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "Kismet/GameplayStatics.h"

ADayManager::ADayManager() {
  PrimaryActorTick.bCanEverTick = false;

  CurrentDay = 0;
}

void ADayManager::BeginPlay() { Super::BeginPlay(); }

void ADayManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ADayManager::StartNewDay() {
  check(Market && MarketLevel && NewsGen);
  UE_LOG(LogTemp, Warning, TEXT("DayManager: Starting new day."));

  CurrentDay++;
  bIsWeekend = CurrentDay % DayManagerParams.WeekendDivisor == 0;

  AbilityManager->TickDaysTimedVars();
  CustomerAIManager->TickDaysTimedVars();
  MarketEconomy->TickDaysActivePriceEffects();
  Market->TickDaysTimedVars();
  MarketLevel->TickDaysTimedVars();
  NewsGen->TickDaysTimedVars();

  StatisticsGen->CalcDayStatistics();

  Market->ConsiderEconEvents();
  NewsGen->GenDaysRandomArticles();

  MarketLevel->ResetLevelState();

  if (CurrentDay % DayManagerParams.DebtPaymentDayDivisor == 0) ManageDebt();
}

void ADayManager::ManageDebt() {
  UE_LOG(LogTemp, Warning, TEXT("DayManager: Managing debt."));

  float DebtAmount = DayManagerParams.BaseDebtAmount *
                     (DayManagerParams.DebtIncreaseMulti * (CurrentDay / DayManagerParams.DebtPaymentDayDivisor));

  if (Store->Money > DebtAmount) {
    StatisticsGen->StoreMoneySpent(DebtAmount);
    return;
  }

  // Game over.
  UE_LOG(LogTemp, Warning, TEXT("DayManager: Game over. Not enough money to pay debt."));
  AStorePGGameMode* GameMode = Cast<AStorePGGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  check(GameMode);
  GameMode->GameOverReset();
}