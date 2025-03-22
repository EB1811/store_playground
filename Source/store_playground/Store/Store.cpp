
#include "Store.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/WorldObject/Buildable.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "store_playground/WorldObject/Buildable.h"

AStore::AStore() {
  PrimaryActorTick.bCanEverTick = false;

  Money = 0;
  StoreStats = {0, 0};
}

void AStore::BeginPlay() {
  Super::BeginPlay();

  check(BuildableClass);
}

void AStore::InitStockDisplays() {
  StoreStockItems.Empty();

  TArray<AActor*> FoundActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), BuildableClass, FoundActors);

  for (AActor* Actor : FoundActors) {
    ABuildable* Buildable = Cast<ABuildable>(Actor);
    check(Buildable);
    if (Buildable->BuildableType != EBuildableType::StockDisplay) continue;

    auto* Stock = Buildable->StockInventory;
    for (UItemBase* Item : Stock->ItemsArray) StoreStockItems.Add({Stock, Item});
  }
}