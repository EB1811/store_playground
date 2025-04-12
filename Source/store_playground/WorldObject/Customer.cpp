#include "Customer.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Quest/QuestComponent.h"

ACustomer::ACustomer() {
  PrimaryActorTick.bCanEverTick = true;

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
  QuestComponent = CreateDefaultSubobject<UQuestComponent>(TEXT("QuestComponent"));
  CustomerAIComponent = CreateDefaultSubobject<UCustomerAIComponent>(TEXT("CustomerAIComponent"));
}

void ACustomer::BeginPlay() { Super::BeginPlay(); }