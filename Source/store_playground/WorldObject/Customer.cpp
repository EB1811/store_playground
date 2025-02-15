#include "Customer.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"

ACustomer::ACustomer() {
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  CustomerAIComponent = CreateDefaultSubobject<UCustomerAIComponent>(TEXT("CustomerAIComponent"));
}

void ACustomer::BeginPlay() {
  Super::BeginPlay();

  InteractionComponent->InteractionType = EInteractionType::Npc;
}