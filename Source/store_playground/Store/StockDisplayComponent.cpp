#include "StockDisplayComponent.h"

UStockDisplayComponent::UStockDisplayComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  DisplayStats.PickRateWeightModifier = 1;
}

void UStockDisplayComponent::BeginPlay() { Super::BeginPlay(); }