#include "NPCStore.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Market/NpcStoreComponent.h"

ANPCStore::ANPCStore() {
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
  InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
  NpcStoreComponent = CreateDefaultSubobject<UNpcStoreComponent>(TEXT("NpcStoreComponent"));
}

void ANPCStore::BeginPlay() { Super::BeginPlay(); }