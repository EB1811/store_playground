#include "NPCStore.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Market/NpcStoreComponent.h"

ANPCStore::ANPCStore() {
  PrimaryActorTick.bCanEverTick = true;

  if (!NpcStoreId.IsValid()) NpcStoreId = FGuid::NewGuid();

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  Mesh->SetSimulatePhysics(true);
  SetRootComponent(Mesh);

  InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
  DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
  InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
  NpcStoreComponent = CreateDefaultSubobject<UNpcStoreComponent>(TEXT("NpcStoreComponent"));
}

void ANPCStore::BeginPlay() { Super::BeginPlay(); }

FNpcStoreSaveState SaveNpcStoreSaveState(ANPCStore* NpcStore) {
  return {
      NpcStore->NpcStoreId,
      NpcStore->DialogueComponent->DialogueArray,
      NpcStore->InventoryComponent->ItemsArray,
      NpcStore->NpcStoreComponent->NpcStoreType,
  };
}

void LoadNpcStoreSaveState(ANPCStore* NpcStore, FNpcStoreSaveState SaveState) {
  NpcStore->NpcStoreId = SaveState.NpcStoreId;
  NpcStore->DialogueComponent->DialogueArray = SaveState.DialogueArray;
  NpcStore->InventoryComponent->ItemsArray = SaveState.ItemsArray;
  NpcStore->NpcStoreComponent->NpcStoreType = SaveState.NpcStoreType;
}