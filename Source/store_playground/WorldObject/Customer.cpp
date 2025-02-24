#include "Customer.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"

ACustomer::ACustomer() {
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  SetRootComponent(Mesh);

  // DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  CustomerAIComponent = CreateDefaultSubobject<UCustomerAIComponent>(TEXT("CustomerAIComponent"));
}

void ACustomer::BeginPlay() {
  Super::BeginPlay();

  // InteractionComponent->InteractionType = EInteractionType::NPCDialogue;
}