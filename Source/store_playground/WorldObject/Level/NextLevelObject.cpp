#include "NextLevelObject.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Level/LevelChangeComponent.h"

ANextLevelObject::ANextLevelObject() {
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  LevelChangeComponent = CreateDefaultSubobject<ULevelChangeComponent>(TEXT("LevelChangeComponent"));
}

void ANextLevelObject::BeginPlay() { Super::BeginPlay(); }