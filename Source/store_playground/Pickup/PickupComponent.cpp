#include "PickupComponent.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"

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

void UPickupComponent::InitPickup(EPickupGoodType _PickupGoodType, float _MoneyAmount, FName _ItemID, float ItemValue) {
  PickupGoodType = _PickupGoodType;
  switch (PickupGoodType) {
    case EPickupGoodType::Item: {
      check(_ItemID != NAME_None);

      ItemID = _ItemID;
      ItemQuantity = 1;
      break;
    }
    case EPickupGoodType::Money: {
      check(_MoneyAmount > 0);

      MoneyAmount = _MoneyAmount;
      break;
    }
    default: checkNoEntry(); break;
  }

  if (SpawnSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), SpawnSound, GetOwner()->GetActorLocation());
}