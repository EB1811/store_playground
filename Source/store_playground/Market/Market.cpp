
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

  TArray<FItemData*> ItemRows;
  AllItemsTable->GetAllRows<FItemData>("", ItemRows);
  for (auto Row : ItemRows) {
    UItemBase* Item = NewObject<UItemBase>(this);

    Item->ItemID = Row->ItemID;
    Item->Quantity = 1;
    Item->UniqueItemID = FGuid::NewGuid();
    Item->FlavorData = Row->FlavorData;
    Item->MetaData = Row->MetaData;
    Item->AssetData = Row->AssetData;
    Item->MarketData = Row->MarketData;

    AllItems.Add(Item);
    MarketPrices.Add(Item->ItemID, Item->MarketData.BasePrice);
  }

  check(NpcStoreDialogues.Num() > 0);
  check(AllItems.Num() > 0);

  NpcStoreDialoguesTable = nullptr;
  AllItemsTable = nullptr;

  // TODO: Move to game mode.
  InitializeNPCStores();
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

TArray<UItemBase*> AMarket::GetNewRandomItems(int32 Amount) {
  TArray<UItemBase*> NewItems;
  for (int32 i = 0; i < Amount; i++) {
    int32 RandomIndex = FMath::RandRange(0, AllItems.Num() - 1);
    NewItems.Add(CreateRandomItem(AllItems[RandomIndex]));
  }

  return NewItems;
}

// TODO: Implement.
UItemBase* AMarket::CreateRandomItem(UItemBase* FromBaseItem) {
  // TODO: Add weights to the random item generation.
  auto BaseItemCopy = FromBaseItem->CreateItemCopy();
  BaseItemCopy->FlavorData.Quality = EItemQuality(FMath::RandRange(0, 3));

  BaseItemCopy->MarketData.CurrentPrice = MarketPrices[BaseItemCopy->ItemID];

  return BaseItemCopy;
}