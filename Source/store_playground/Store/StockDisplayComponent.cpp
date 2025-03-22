#include "StockDisplayComponent.h"

UStockDisplayComponent::UStockDisplayComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  DisplayStats.PickRateModifier = 1.0f;
}

void UStockDisplayComponent::BeginPlay() { Super::BeginPlay(); }