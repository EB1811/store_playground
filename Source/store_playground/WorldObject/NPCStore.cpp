#include "NPCStore.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Market/NpcStoreComponent.h"

ANPCStore::ANPCStore() {
  PrimaryActorTick.bCanEverTick = true;

  if (!NpcStoreId.IsValid()) NpcStoreId = FGuid::NewGuid();

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

void ANPCStore::BeginPlay() { Super::BeginPlay(); }