#include "MiniGame.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Minigame/MiniGameComponent.h"

AMiniGame::AMiniGame() {
  PrimaryActorTick.bCanEverTick = true;

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
  MiniGameComponent = CreateDefaultSubobject<UMiniGameComponent>(TEXT("MiniGameComponent"));
}

void AMiniGame::BeginPlay() { Super::BeginPlay(); }