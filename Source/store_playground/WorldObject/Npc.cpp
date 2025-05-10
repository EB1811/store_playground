#include "Npc.h"
#include "Components/StaticMeshComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Quest/QuestComponent.h"
#include "store_playground/Sprite/SimpleSpriteAnimComponent.h"

ANpc::ANpc() {
  PrimaryActorTick.bCanEverTick = true;

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
  QuestComponent = CreateDefaultSubobject<UQuestComponent>(TEXT("QuestComponent"));
  SimpleSpriteAnimComponent = CreateDefaultSubobject<USimpleSpriteAnimComponent>(TEXT("SimpleSpriteAnimComponent"));
}

void ANpc::BeginPlay() { Super::BeginPlay(); }