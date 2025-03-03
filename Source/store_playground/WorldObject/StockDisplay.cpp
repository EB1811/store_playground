#include "StockDisplay.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"

AStockDisplay::AStockDisplay() {
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
  StockDisplayComponent = CreateDefaultSubobject<UStockDisplayComponent>(TEXT("StockDisplayComponent"));
}

void AStockDisplay::BeginPlay() { Super::BeginPlay(); }