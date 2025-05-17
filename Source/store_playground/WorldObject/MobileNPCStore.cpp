#include "MobileNPCStore.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Market/NpcStoreComponent.h"
#include "PaperFlipbookComponent.h"

AMobileNPCStore::AMobileNPCStore() {
  PrimaryActorTick.bCanEverTick = true;

  SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
  SetRootComponent(SceneRoot);

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetupAttachment(SceneRoot);
  Mesh->SetSimulatePhysics(true);

  Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
  Sprite->SetupAttachment(SceneRoot);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
  InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
  NpcStoreComponent = CreateDefaultSubobject<UNpcStoreComponent>(TEXT("NpcStoreComponent"));
}

void AMobileNPCStore::BeginPlay() { Super::BeginPlay(); }