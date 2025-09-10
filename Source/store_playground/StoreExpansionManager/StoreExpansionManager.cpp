#include "StoreExpansionManager.h"
#include "store_playground/Level/LevelManager.h"
#include "store_playground/Store/Store.h"

void AStoreExpansionManager::BeginPlay() { Super::BeginPlay(); }

void AStoreExpansionManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

// TODO: Move all items in displays to inventory upon expansion.
void AStoreExpansionManager::SelectExpansion(FName StoreExpansionLevelID) {
  FStoreExpansionData* ExpansionData = StoreExpansions.FindByPredicate(
      [&](const FStoreExpansionData& Data) { return Data.StoreExpansionLevelID == StoreExpansionLevelID; });
  check(ExpansionData);
  check(Store->Money >= ExpansionData->Price && !ExpansionData->bIsLocked);

  CurrentStoreExpansionLevelID = StoreExpansionLevelID;
  LevelManager->ExpandStoreSwitchLevel();  // TODO: Add post load callback for UI.
}

void AStoreExpansionManager::UnlockIDs(const FName DataName, const TArray<FName>& Ids) {
  if (DataName != "StoreExpansions") checkf(false, TEXT("UnlockIDs only supports StoreExpansions."));

  if (DataName == "StoreExpansions") {
    for (const auto& Id : Ids) {
      FStoreExpansionData* ExpansionData = StoreExpansions.FindByPredicate(
          [&](const FStoreExpansionData& Data) { return Data.StoreExpansionLevelID == Id; });
      if (ExpansionData) ExpansionData->bIsLocked = false;
    }
  }
}