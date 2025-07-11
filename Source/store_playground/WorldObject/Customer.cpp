#include "Customer.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Quest/QuestComponent.h"
#include "store_playground/Sprite/SimpleSpriteAnimComponent.h"
#include "Components/WidgetComponent.h"

ACustomer::ACustomer() {
  PrimaryActorTick.bCanEverTick = true;

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
  QuestComponent = CreateDefaultSubobject<UQuestComponent>(TEXT("QuestComponent"));
  CustomerAIComponent = CreateDefaultSubobject<UCustomerAIComponent>(TEXT("CustomerAIComponent"));
  SimpleSpriteAnimComponent = CreateDefaultSubobject<USimpleSpriteAnimComponent>(TEXT("SimpleSpriteAnimComponent"));
  WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
}

void ACustomer::BeginPlay() {
  Super::BeginPlay();

  // WidgetComponent->SetVisibility(false, true);
  // WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
  // WidgetComponent->SetDrawSize(FVector2D(100.0f, 100.0f));
  WidgetComponent->SetWorldLocation(GetActorLocation() + FVector(0, -35.0f, 110.0f));
  WidgetComponent->SetWorldRotation(FRotator(35, 90, 0));  // y, z, x
}

void ACustomer::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  if (WidgetComponent->IsWidgetVisible())
    WidgetComponent->SetWorldLocation(GetActorLocation() + FVector(0, -35.0f, 110.0f));
}