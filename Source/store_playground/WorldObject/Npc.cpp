#include "Npc.h"
#include "Components/StaticMeshComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Quest/QuestComponent.h"
#include "store_playground/Sprite/SimpleSpriteAnimComponent.h"
#include "Components/WidgetComponent.h"

ANpc::ANpc() {
  PrimaryActorTick.bCanEverTick = true;

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
  QuestComponent = CreateDefaultSubobject<UQuestComponent>(TEXT("QuestComponent"));
  SimpleSpriteAnimComponent = CreateDefaultSubobject<USimpleSpriteAnimComponent>(TEXT("SimpleSpriteAnimComponent"));
  DynamicTalkingWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DynamicTalkingWidgetComponent"));
}

void ANpc::BeginPlay() {
  Super::BeginPlay();

  // DynamicTalkingWidgetComponent->SetVisibility(false, true);
  // DynamicTalkingWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
  // DynamicTalkingWidgetComponent->SetDrawSize(FVector2D(100.0f, 100.0f));
  DynamicTalkingWidgetComponent->SetWorldLocation(GetActorLocation() + FVector(0, -35.0f, 100.0f));
  DynamicTalkingWidgetComponent->SetWorldRotation(FRotator(35, 90, 0));  // y, z, x
}