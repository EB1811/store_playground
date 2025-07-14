
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
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Lighting/StorePhaseLightingManager.h"
#include "store_playground/Sound/AmbientSoundManager.h"

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

auto AStore::BuildStockDisplay(ABuildable* Buildable) -> bool {
  if (Buildable->BuildingPricesMap[EBuildableType::StockDisplay] > Money) return false;

  Buildable->SetToStockDisplay();
  if (Buildable->BuildableType != EBuildableType::StockDisplay) return false;

  MoneySpent(Buildable->BuildingPricesMap[EBuildableType::StockDisplay]);
  return true;
}
auto AStore::BuildDecoration(ABuildable* Buildable) -> bool {
  if (Buildable->BuildingPricesMap[EBuildableType::Decoration] > Money) return false;

  Buildable->SetToDecoration();
  if (Buildable->BuildableType != EBuildableType::Decoration) return false;

  MoneySpent(Buildable->BuildingPricesMap[EBuildableType::Decoration]);
  return true;
}

auto AStore::TrySpendMoney(float Amount) -> bool {
  if (Amount > Money) return false;

  MoneySpent(Amount);
  return true;
}

void AStore::StockItemSold(const UItemBase* Item) {
  auto* StockItem = StoreStockItems.FindByPredicate(
      [Item](const FStockItem& StockItem) { return StockItem.Item->UniqueItemID == Item->UniqueItemID; });
  check(StockItem);

  StockItem->StockDisplayComponent->ClearDisplaySprite();
  StockItem->BelongingStockInventoryC->RemoveItem(Item);
  StoreStockItems.RemoveAllSwap(
      [Item](const FStockItem& StockItem) { return StockItem.Item->UniqueItemID == Item->UniqueItemID; });
}

void AStore::ItemBought(UItemBase* Item, float SingleUnitPrice, int32 Quantity) {
  checkf((SingleUnitPrice * Quantity) <= Money,
         TEXT("Caller should handle the case when Price is greater than available Money."));

  Item->PlayerPriceData.BoughtAt = SingleUnitPrice;

  Money -= SingleUnitPrice * Quantity;

  StatisticsGen->StoreMoneySpent(SingleUnitPrice * Quantity);
}
void AStore::ItemSold(const UItemBase* Item, float SingleUnitPrice, int32 Quantity) {
  Money += SingleUnitPrice * Quantity;

  StatisticsGen->ItemDeal({Item->ItemID, Item->PlayerPriceData.BoughtAt, SingleUnitPrice, Quantity});
  StatisticsGen->StoreMoneyGained(SingleUnitPrice * Quantity);
}
void AStore::MoneyGained(float Amount) {
  Money += Amount;

  StatisticsGen->StoreMoneyGained(Amount);
}
void AStore::MoneySpent(float Amount) {
  Money -= Amount;

  StatisticsGen->StoreMoneySpent(Amount);
}

void AStore::SetupStoreEnvironment() {
  switch (StorePhaseManager->StorePhaseState) {
    case EStorePhaseState::None: break;
    case EStorePhaseState::Morning:
      StorePhaseLightingManager->OnEndDayLightingCalled();
      AmbientSoundManager->MorningSoundCalled();
      break;
    case EStorePhaseState::ShopOpen:
      StorePhaseLightingManager->OnOpenShopLightingCalled();
      AmbientSoundManager->ShopOpenSoundCalled();
      break;
    case EStorePhaseState::Night:
      StorePhaseLightingManager->OnCloseShopLightingCalled();
      AmbientSoundManager->NightSoundCalled();
      break;
    default: checkNoEntry();
  }
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
      LoadBuildableSaveState(Buildable);

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

  SetupStoreEnvironment();
  InitStockDisplays();
}

void AStore::InitStockDisplays() {
  StoreStockItems.Empty();

  TArray<ABuildable*> FoundBuildables = GetAllActorsOf<ABuildable>(GetWorld(), BuildableClass);
  for (ABuildable* Buildable : FoundBuildables) {
    if (Buildable->BuildableType != EBuildableType::StockDisplay) continue;

    auto* StockInventory = Buildable->StockInventory;
    for (UItemBase* Item : StockInventory->ItemsArray) {
      StoreStockItems.Add({Item->ItemID, Buildable->StockDisplay, Item, StockInventory});
      Buildable->StockDisplay->SetDisplaySprite(Item->AssetData.Sprite);
    }
  }
}