
#include "Market.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/WorldObject/NPCStore.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AMarket::AMarket() { PrimaryActorTick.bCanEverTick = false; }

void AMarket::BeginPlay() {
  Super::BeginPlay();

  check(NpcStoreDialoguesTable && AllItemsTable && NPCStoreClass);

  TArray<FDialogueDataTable*> NpcStoreDialoguesRows;
  NpcStoreDialoguesTable->GetAllRows<FDialogueDataTable>("", NpcStoreDialoguesRows);
  for (auto* Row : NpcStoreDialoguesRows)
    NpcStoreDialogues.Add({
        Row->DialogueChainID,
        Row->DialogueType,
        Row->DialogueText,
        Row->Action,
        Row->DialogueSpeaker,
        Row->ChoicesAmount,
    });

  TArray<FItemDataRow*> ItemRows;
  AllItemsTable->GetAllRows<FItemDataRow>("", ItemRows);
  for (auto Row : ItemRows) {
    UItemBase* Item = NewObject<UItemBase>(this);

    Item->ItemID = Row->ItemID;
    Item->UniqueItemID = FGuid::NewGuid();
    Item->Quantity = 1;
    Item->ItemType = Row->ItemType;
    Item->ItemWealthType = Row->ItemWealthType;
    Item->ItemEconType = Row->ItemEconType;
    Item->TextData = Row->TextData;
    Item->AssetData = Row->AssetData;
    Item->FlavorData = Row->FlavorData;
    Item->PriceData.BoughtAt = Row->BasePrice;

    AllItems.Add(Item);
  }

  check(NpcStoreDialogues.Num() > 0);
  check(AllItems.Num() > 0);

  NpcStoreDialoguesTable = nullptr;
  AllItemsTable = nullptr;
}

void AMarket::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AMarket::InitializeNPCStores() {
  TArray<AActor*> FoundActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), NPCStoreClass, FoundActors);

  for (AActor* Actor : FoundActors) {
    ANPCStore* NPCStore = Cast<ANPCStore>(Actor);
    check(NPCStore);

    TArray<int32> RandomDialogueIndexes = GetRandomDialogueIndexes();
    for (int32 RandomDialogue : RandomDialogueIndexes)
      NPCStore->DialogueComponent->DialogueArray.Add(NpcStoreDialogues[RandomDialogue]);
    NPCStore->InventoryComponent->ItemsArray = GetNewRandomItems(2);
  }
}

TArray<int32> AMarket::GetRandomDialogueIndexes() {
  TArray<int32> RandomDialogueIndexes;

  int32 MapSize = NpcStoreDialogues.Num();
  int32 RandomIndex = FMath::RandRange(0, MapSize - 1);
  for (int32 i = RandomIndex - 1; i >= 0; i--) {
    if (NpcStoreDialogues[i].DialogueChainID != NpcStoreDialogues[RandomIndex].DialogueChainID) break;
    RandomIndex = i;
  }

  for (int32 i = RandomIndex; i < MapSize; i++) {
    if (NpcStoreDialogues[i].DialogueChainID != NpcStoreDialogues[RandomIndex].DialogueChainID) break;
    RandomDialogueIndexes.Add(i);
  }

  return RandomDialogueIndexes;
}

// ? Get items based on factors?
TArray<UItemBase*> AMarket::GetNewRandomItems(int32 Amount) const {
  TArray<UItemBase*> NewItems;
  for (int32 i = 0; i < Amount; i++) {
    int32 RandomIndex = FMath::RandRange(0, AllItems.Num() - 1);
    NewItems.Add(AllItems[RandomIndex]->CreateItemCopy());
  }

  return NewItems;
}

UItemBase* AMarket::GetItemByID(const FName& ItemID) const {
  return *(AllItems.FindByPredicate([ItemID](const UItemBase* Item) { return Item->ItemID == ItemID; }));
}