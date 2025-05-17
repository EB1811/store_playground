#include "StoreExpansionManager.h"
#include "store_playground/Store/Store.h"

void AStoreExpansionManager::BeginPlay() { Super::BeginPlay(); }

void AStoreExpansionManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

auto AStoreExpansionManager::SelectExpansion(EStoreExpansionLevel StoreExpansionLevel) -> bool {
  FStoreExpansionData* ExpansionData = StoreExpansions.FindByPredicate(
      [&](const FStoreExpansionData& Data) { return Data.StoreExpansionLevel == StoreExpansionLevel; });
  check(ExpansionData);

  if (!Store->TrySpendMoney(ExpansionData->Price)) return false;

  CurrentStoreExpansionLevel = StoreExpansionLevel;
  return true;
}