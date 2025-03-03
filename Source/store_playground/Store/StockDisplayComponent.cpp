#include "StockDisplayComponent.h"

UStockDisplayComponent::UStockDisplayComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  DisplayStats.PriceIncreasePercentage = 0.0f;
}

void UStockDisplayComponent::BeginPlay() { Super::BeginPlay(); }