#include "StorePhaseTrigger.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "Components/WidgetComponent.h"

AStorePhaseTrigger::AStorePhaseTrigger() {
  PrimaryActorTick.bCanEverTick = false;

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  InteractionComponent->InteractionType = EInteractionType::StoreNextPhase;
}

void AStorePhaseTrigger::BeginPlay() {
  Super::BeginPlay();

  check(InteractionComponent->InteractionType == EInteractionType::StoreNextPhase);
}