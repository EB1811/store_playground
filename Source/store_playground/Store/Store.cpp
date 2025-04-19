
#include "Store.h"
#include "Containers/Map.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/WorldObject/Buildable.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "store_playground/WorldObject/Buildable.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"

AStore::AStore() {
  PrimaryActorTick.bCanEverTick = false;

  Money = 0;
  StoreStats = {0, 0};
}

void AStore::BeginPlay() {
  Super::BeginPlay();

  check(BuildableClass);
}

void AStore::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

// TODO: change all direct store changes to using this.
void AStore::ItemBought(UItemBase* Item, float Price, int32 Quantity) {
  Money -= Price * Quantity;
  Item->PriceData.BoughtAt = Price;

  StatisticsGen->StoreMoneySpent(Price * Quantity);
}
void AStore::ItemSold(const UItemBase* Item, float Price, int32 Quantity) {
  Money += Price * Quantity;
  StoreStockItems.RemoveAllSwap(
      [Item](const FStockItem& StockItem) { return StockItem.Item->UniqueItemID == Item->UniqueItemID; });

  StatisticsGen->ItemDeal({Item->ItemID, Item->PriceData.BoughtAt, Price, Quantity});
  StatisticsGen->StoreMoneyGained(Price * Quantity);
}
void AStore::MoneyGained(float Amount) {
  Money += Amount;

  StatisticsGen->StoreMoneyGained(Amount);
}
void AStore::MoneySpent(float Amount) {
  Money -= Amount;

  StatisticsGen->StoreMoneySpent(Amount);
}

void AStore::SaveStoreLevelState() {
  StoreLevelState.ActorSaveMap.Empty();
  StoreLevelState.ComponentSaveMap.Empty();
  StoreLevelState.ObjectSaveStates.Empty();

  TArray<ABuildable*> FoundBuildables = GetAllActorsOf<ABuildable>(GetWorld(), BuildableClass);

  for (ABuildable* Buildable : FoundBuildables) {
    FActorSavaState SaveState = SaveManager->SaveActor(Buildable, Buildable->BuildableId);

    auto [ComponentSaveState, FObjectSaveStates] = SaveManager->SaveInventoryCSaveState(Buildable->StockInventory);
    SaveState.ActorComponentsMap.Add("InventoryComponent", ComponentSaveState.Id);

    StoreLevelState.ActorSaveMap.Add(Buildable->BuildableId, SaveState);
    StoreLevelState.ComponentSaveMap.Add(ComponentSaveState.Id, ComponentSaveState);
    for (FObjectSaveState& ObjectSaveState : FObjectSaveStates) StoreLevelState.ObjectSaveStates.Add(ObjectSaveState);
    UE_LOG(LogTemp, Warning, TEXT("ComponentSaveState.ComponentObjects number: %d"),
           ComponentSaveState.ComponentObjects.Num());
  }

  // StoreLevelState.BinaryMap.Empty();
  // for (int32 i = 0; i < BSaveStates.Num(); i++) {
  //   UStruct* SaveStateStruct = FBuildableSaveState::StaticStruct();

  //   FBuildableBinary BuildableBinary;
  //   FMemoryWriter MemWriter(BuildableBinary.BuildableByteData);
  //   FBuildableSaveState::StaticStruct()->SerializeBin(MemWriter, &BSaveStates[i]);

  //   StoreLevelState.BinaryMap.Add(FoundBuildables[i]->BuildableId, BuildableBinary);
  // }
}

void AStore::LoadStoreLevelState() {
  if (StoreLevelState.ActorSaveMap.Num() > 0) {
    TArray<ABuildable*> FoundBuildables = GetAllActorsOf<ABuildable>(GetWorld(), BuildableClass);
    for (ABuildable* Buildable : FoundBuildables) {
      check(StoreLevelState.ActorSaveMap.Contains(Buildable->BuildableId));

      FActorSavaState ActorSaveState = StoreLevelState.ActorSaveMap[Buildable->BuildableId];
      FMemoryReader MemReader(ActorSaveState.ByteData);
      FObjectAndNameAsStringProxyArchive Ar(MemReader, true);

      Ar.ArIsSaveGame = true;
      Buildable->Serialize(Ar);
      LoadBuildableSaveState(Buildable);

      FComponentSaveState ComponentSaveState =
          StoreLevelState.ComponentSaveMap[ActorSaveState.ActorComponentsMap["InventoryComponent"]];
      TArray<FObjectSaveState> ComponentObjectSaveStates = StoreLevelState.ObjectSaveStates.FilterByPredicate(
          [ComponentSaveState](const FObjectSaveState& ObjectSaveState) {
            return ComponentSaveState.ComponentObjects.Contains(ObjectSaveState.Id);
          });
      UE_LOG(LogTemp, Warning, TEXT("ComponentObjectSaveStates number: %d"), ComponentObjectSaveStates.Num());
      SaveManager->LoadInventoryCSaveState(Buildable->StockInventory, ComponentSaveState, ComponentObjectSaveStates);

      // FBuildableBinary BuildableBinary = StoreLevelState.BinaryMap[Buildable->BuildableId];
      // FBuildableSaveState BuildableSaveState;
      // FMemoryReader MemReader(BuildableBinary.BuildableByteData);
      // FBuildableSaveState::StaticStruct()->SerializeBin(MemReader, &BuildableSaveState);

      // LoadBuildableSaveState(Buildable, BuildableSaveState);
    }
  }

  InitStockDisplays();
}

void AStore::InitStockDisplays() {
  StoreStockItems.Empty();

  TArray<ABuildable*> FoundBuildables = GetAllActorsOf<ABuildable>(GetWorld(), BuildableClass);
  for (ABuildable* Buildable : FoundBuildables) {
    if (Buildable->BuildableType != EBuildableType::StockDisplay) continue;

    auto* StockInventory = Buildable->StockInventory;
    for (UItemBase* Item : StockInventory->ItemsArray)
      StoreStockItems.Add({Buildable->StockDisplay->DisplayStats, Item, StockInventory});
  }
}