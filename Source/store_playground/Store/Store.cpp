
#include "Store.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/WorldObject/Buildable.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "store_playground/WorldObject/Buildable.h"
#include "store_playground/Framework/UtilFuncs.h"

AStore::AStore() {
  PrimaryActorTick.bCanEverTick = false;

  Money = 0;
  StoreStats = {0, 0};
}

void AStore::BeginPlay() {
  Super::BeginPlay();

  check(BuildableClass);
}
void AStore::SaveStoreLevelState() {
  StoreLevelState.BuildableSaveMap.Empty();

  TArray<ABuildable*> FoundBuildables = GetAllActorsOf<ABuildable>(GetWorld(), BuildableClass);
  for (ABuildable* Buildable : FoundBuildables)
    StoreLevelState.BuildableSaveMap.Add(Buildable->BuildableId, SaveBuildableSaveState(Buildable));
}

void AStore::LoadStoreLevelState() {
  if (StoreLevelState.BuildableSaveMap.Num() == 0) return InitStockDisplays();

  TArray<ABuildable*> FoundBuildables = GetAllActorsOf<ABuildable>(GetWorld(), BuildableClass);
  for (ABuildable* Buildable : FoundBuildables) {
    check(StoreLevelState.BuildableSaveMap.Contains(Buildable->BuildableId));
    LoadBuildableSaveState(Buildable, StoreLevelState.BuildableSaveMap[Buildable->BuildableId]);
  }

  InitStockDisplays();
}

void AStore::InitStockDisplays() {
  StoreStockItems.Empty();

  TArray<ABuildable*> FoundBuildables = GetAllActorsOf<ABuildable>(GetWorld(), BuildableClass);
  for (ABuildable* Buildable : FoundBuildables) {
    if (Buildable->BuildableType != EBuildableType::StockDisplay) continue;

    auto* StockInventory = Buildable->StockInventory;
    for (UItemBase* Item : StockInventory->ItemsArray)
      StoreStockItems.Add({Buildable->StockDisplay->DisplayStats, Item, StockInventory});
  }
}