#include "MiniGame.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Minigame/MiniGameComponent.h"
#include "PaperFlipbookComponent.h"

AMiniGame::AMiniGame() {
  PrimaryActorTick.bCanEverTick = true;

  SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
  SetRootComponent(SceneRoot);

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetupAttachment(SceneRoot);

  Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
  Sprite->SetupAttachment(SceneRoot);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
  MiniGameComponent = CreateDefaultSubobject<UMiniGameComponent>(TEXT("MiniGameComponent"));
}

void AMiniGame::BeginPlay() { Super::BeginPlay(); }