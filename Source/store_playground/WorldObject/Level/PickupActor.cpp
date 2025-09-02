#include "PickupActor.h"
#include "Components/StaticMeshComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Pickup/PickupComponent.h"
#include "Kismet/GameplayStatics.h"

APickupActor::APickupActor() {
  PrimaryActorTick.bCanEverTick = false;

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  PickupComponent = CreateDefaultSubobject<UPickupComponent>(TEXT("PickupComponent"));
}

void APickupActor::BeginPlay() {
  Super::BeginPlay();

  if (PickupComponent->SpawnSound)
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupComponent->SpawnSound, GetActorLocation());
}

void APickupActor::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void APickupActor::InitPickup(EPickupGoodType PickupGoodType, float MoneyAmount, FName ItemID, float ItemValue) {
  PickupComponent->PickupGoodType = PickupGoodType;
  switch (PickupGoodType) {
    case EPickupGoodType::Item: {
      check(ItemID != NAME_None);

      PickupComponent->ItemID = ItemID;
      PickupComponent->ItemQuantity = 1;
      break;
    }
    case EPickupGoodType::Money: {
      check(MoneyAmount > 0);

      PickupComponent->MoneyAmount = MoneyAmount;
      break;
    }
    default: checkNoEntry(); break;
  }
}