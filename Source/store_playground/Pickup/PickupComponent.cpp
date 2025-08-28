#include "PickupComponent.h"
#include "Components/ActorComponent.h"

UPickupComponent::UPickupComponent() { PrimaryComponentTick.bCanEverTick = false; }

void UPickupComponent::BeginPlay() {
  Super::BeginPlay();

  switch (PickupGoodType) {
    case EPickupGoodType::Item: check(ItemID != NAME_None && ItemQuantity > 0); break;
    case EPickupGoodType::Money: check(MoneyAmount > 0.0f); break;
    default: checkNoEntry(); break;
  }
}

void UPickupComponent::DestroyPickup() { GetOwner()->Destroy(); }