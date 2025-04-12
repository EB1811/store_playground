
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

AStore::AStore() {
  PrimaryActorTick.bCanEverTick = false;

  Money = 0;
  StoreStats = {0, 0};
}

void AStore::BeginPlay() {
  Super::BeginPlay();

  check(BuildableClass);
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
      LoadBuildableSaveState(Buildable, {});

      FComponentSaveState ComponentSaveState =
          StoreLevelState.ComponentSaveMap[ActorSaveState.ActorComponentsMap["InventoryComponent"]];
      TArray<FObjectSaveState> ComponentObjectSaveStates = StoreLevelState.ObjectSaveStates.FilterByPredicate(
          [ComponentSaveState](const FObjectSaveState& ObjectSaveState) {
            return ComponentSaveState.ComponentObjects.Contains(ObjectSaveState.Id);
          });
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