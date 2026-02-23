#include "StoreExpansionManager.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/Level/LevelManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Inventory/InventoryComponent.h"

void AStoreExpansionManager::BeginPlay() { Super::BeginPlay(); }

void AStoreExpansionManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AStoreExpansionManager::SelectExpansion(FName StoreExpansionLevelID) {
  FStoreExpansionData* ExpansionData = StoreExpansions.FindByPredicate(
      [&](const FStoreExpansionData& Data) { return Data.StoreExpansionLevelID == StoreExpansionLevelID; });
  check(ExpansionData);
  check(Store->GetAvailableMoney() >= ExpansionData->Price && !ExpansionData->bIsLocked);
  check(LevelManager->CurrentLevel == ELevel::Store);
  check(StorePhaseManager->StorePhaseState != EStorePhaseState::ShopOpen);

  Store->MoneySpent(ExpansionData->Price);
  for (auto StockItem : Store->StoreStockItems)
    TransferItem(StockItem.BelongingStockInventoryC, PlayerInventoryC, StockItem.Item, StockItem.Item->Quantity);

  if (StorePhaseManager->StorePhaseState == EStorePhaseState::MorningBuildMode) StorePhaseManager->ToggleBuildMode();

  CurrentStoreExpansionLevelID = StoreExpansionLevelID;
  LevelManager->ExpandStoreSwitchLevel();
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