#include "UpgradePoint.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Upgrade/UpgradeSelectComponent.h"

AUpgradePoint::AUpgradePoint() {
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  UpgradeSelectComponent = CreateDefaultSubobject<UUpgradeSelectComponent>(TEXT("UpgradeSelectComponent"));
}

void AUpgradePoint::BeginPlay() { Super::BeginPlay(); }