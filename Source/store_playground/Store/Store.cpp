
#include "Store.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"

AStore::AStore() {
  PrimaryActorTick.bCanEverTick = false;

  Money = 0;
  StoreStats = {0, 0};

  StoreStock = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void AStore::BeginPlay() { Super::BeginPlay(); }