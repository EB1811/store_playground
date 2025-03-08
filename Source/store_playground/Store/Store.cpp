
#include "Store.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "store_playground/WorldObject/StockDisplay.h"

AStore::AStore() {
  PrimaryActorTick.bCanEverTick = false;

  Money = 0;
  StoreStats = {0, 0};
}

void AStore::BeginPlay() { Super::BeginPlay(); }

void AStore::InitStockDisplays() {
  TArray<AActor*> FoundActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), StockDisplayClass, FoundActors);

  for (AActor* Actor : FoundActors) {
    AStockDisplay* StockDisplay = Cast<AStockDisplay>(Actor);
    check(StockDisplay);

    StockDisplay->InventoryComponent = StockDisplay->InventoryComponent;
    for (UItemBase* Item : StockDisplay->InventoryComponent->ItemsArray)

      StoreStockItems.Add({StockDisplay->InventoryComponent, Item});
  }
}