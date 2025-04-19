#include "AbilityPoint.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Level/LevelChangeComponent.h"

AAbilityPoint::AAbilityPoint() {
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
}

void AAbilityPoint::BeginPlay() { Super::BeginPlay(); }