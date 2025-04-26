#include "StoreExpansionManager.h"
#include "store_playground/Store/Store.h"

void AStoreExpansionManager::BeginPlay() { Super::BeginPlay(); }

void AStoreExpansionManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AStoreExpansionManager::SelectExpansion(EStoreExpansionLevel StoreExpansionLevel) {
  FStoreExpansionData* ExpansionData = StoreExpansions.FindByPredicate(
      [&](const FStoreExpansionData& Data) { return Data.StoreExpansionLevel == StoreExpansionLevel; });
  check(ExpansionData);

  Store->MoneySpent(ExpansionData->Price);
  CurrentStoreExpansionLevel = StoreExpansionLevel;
}