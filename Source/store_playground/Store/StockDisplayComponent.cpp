#include "StockDisplayComponent.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

UStockDisplayComponent::UStockDisplayComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  DisplayStats.PickRateWeightModifier = 1;
}

void UStockDisplayComponent::BeginPlay() { Super::BeginPlay(); }

void UStockDisplayComponent::SetDisplaySprite(UPaperFlipbook* ItemSprite) {
  check(ItemSprite);

  UPaperFlipbookComponent* Sprite = GetOwner()->FindComponentByClass<UPaperFlipbookComponent>();
  Sprite->SetFlipbook(ItemSprite);
}

void UStockDisplayComponent::ClearDisplaySprite() {
  UPaperFlipbookComponent* Sprite = GetOwner()->FindComponentByClass<UPaperFlipbookComponent>();
  Sprite->SetFlipbook(nullptr);
}