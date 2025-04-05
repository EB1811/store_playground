#include "DayManager.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "store_playground/AI/CustomerAIManager.h"

ADayManager::ADayManager() {
  PrimaryActorTick.bCanEverTick = false;

  CurrentDay = 0;
}

void ADayManager::BeginPlay() { Super::BeginPlay(); }

void ADayManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ADayManager::StartNewDay() {
  check(Market && NewsGen);
  UE_LOG(LogTemp, Warning, TEXT("DayManager: Starting new day."));

  CurrentDay++;

  CustomerAIManager->TickDaysTimedVars();
  MarketEconomy->TickDaysActivePriceEffects();
  Market->TickDaysTimedVars();
  NewsGen->TickDaysTimedVars();

  TArray<struct FEconEvent> EconEvents = Market->ConsiderEconEvents();

  TArray<FName> GuaranteedArticles;
  for (const FEconEvent& EconEvent : EconEvents)
    if (!EconEvent.ArticleID.IsNone()) GuaranteedArticles.Add(EconEvent.ArticleID);
  NewsGen->GenDaysRandomArticles(GuaranteedArticles);

  Market->ResetMarketLevelState();
}