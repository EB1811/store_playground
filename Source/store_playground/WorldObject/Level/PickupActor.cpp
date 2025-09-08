#include "PickupActor.h"
#include "Components/StaticMeshComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Pickup/PickupComponent.h"
#include "Components/AudioComponent.h"

APickupActor::APickupActor() {
  PrimaryActorTick.bCanEverTick = false;

  if (!Id.IsValid()) Id = FGuid::NewGuid();

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  PickupComponent = CreateDefaultSubobject<UPickupComponent>(TEXT("PickupComponent"));
  AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

  AudioComponent->SetupAttachment(RootComponent);
  AudioComponent->bAutoActivate = true;
}

void APickupActor::BeginPlay() { Super::BeginPlay(); }

void APickupActor::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
