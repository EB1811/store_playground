#include "SaveManager.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Level/LevelManager.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/Quest/QuestManager.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/MySaveGame.h"
#include "store_playground/Player/PlayerZDCharacter.h"
#include "Containers/Map.h"
#include "GameFramework/Actor.h"
#include "UObject/ObjectMacros.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Kismet/GameplayStatics.h"

template <typename T>
void SetStructNonSaveGameProperties(UScriptStruct* StaticStruct, T& Current, T& Former) {
  for (TFieldIterator<FProperty> It(StaticStruct); It; ++It) {
    FProperty* Property = *It;
    if (Property->HasAnyPropertyFlags(CPF_SaveGame)) continue;

    if (Property->IsA<FFloatProperty>()) {
      float* PropValuePtr = Property->ContainerPtrToValuePtr<float>(&Current);
      *PropValuePtr = *Property->ContainerPtrToValuePtr<float>(&Former);
    } else if (auto NameProp = CastField<FNameProperty>(Property)) {
      FName* PropValuePtr = NameProp->ContainerPtrToValuePtr<FName>(&Current);
      *PropValuePtr = *NameProp->ContainerPtrToValuePtr<FName>(&Former);
    } else if (auto EnumProp = CastField<FEnumProperty>(Property)) {
      FNumericProperty* UnderlyingProp = EnumProp->GetUnderlyingProperty();
      uint8* PropValuePtr = EnumProp->ContainerPtrToValuePtr<uint8>(&Current);
      *PropValuePtr = *EnumProp->ContainerPtrToValuePtr<uint8>(&Former);
    }
  }
}

ASaveManager::ASaveManager() { PrimaryActorTick.bCanEverTick = false; }

void ASaveManager::BeginPlay() { Super::BeginPlay(); }

void ASaveManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ASaveManager::CreateNewSaveGame() {
  // Having two slots for backup.
  FString SlotName = "SaveSlot_Main";
  FString BackupSlotName = "SaveSlot_Backup";

  CurrentSaveGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
  check(CurrentSaveGame);

  CurrentSaveGame->Initialize(SlotName);
  SaveCurrentSlotToDisk();
}

void ASaveManager::SaveCurrentSlotToDisk() {
  check(CurrentSaveGame);

  CurrentSaveGame->SystemSaveStates = SaveAllSystems();

  FLevelsSaveData LevelsSaveData = SaveLevels();
  for (auto& Pair : LevelsSaveData.LevelSaveMap) CurrentSaveGame->LevelSaveStates.Add(Pair.Value);
  for (auto& Pair : LevelsSaveData.ActorSaveMap) CurrentSaveGame->ActorSaveStates.Add(Pair.Value);
  for (auto& Pair : LevelsSaveData.ComponentSaveMap) CurrentSaveGame->ComponentSaveStates.Add(Pair.Value);
  CurrentSaveGame->ObjectSaveStates.Append(LevelsSaveData.ObjectSaveStates);

  auto [PlayerSaveState, PComponentSaveState, PObjectSaveStates] = SavePlayer();
  CurrentSaveGame->PlayerSaveState = PlayerSaveState;
  CurrentSaveGame->ComponentSaveStates.Add(PComponentSaveState);
  CurrentSaveGame->ObjectSaveStates.Append(PObjectSaveStates);

  UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSaveGame->SlotName, 0);
}

void ASaveManager::LoadCurrentSlotFromDisk() {
  // Having two slots for backup.
  FString SlotName = "SaveSlot_Main";

  CurrentSaveGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
  check(CurrentSaveGame);

  LoadAllSystems(CurrentSaveGame->SystemSaveStates);

  FLevelsSaveData LevelsSaveData;
  for (auto& Pair : CurrentSaveGame->LevelSaveStates) LevelsSaveData.LevelSaveMap.Add(Pair.Name, Pair);
  for (auto& Pair : CurrentSaveGame->ActorSaveStates) LevelsSaveData.ActorSaveMap.Add(Pair.Id, Pair);
  for (auto& Pair : CurrentSaveGame->ComponentSaveStates) LevelsSaveData.ComponentSaveMap.Add(Pair.Id, Pair);
  LevelsSaveData.ObjectSaveStates = CurrentSaveGame->ObjectSaveStates;
  LoadLevels(LevelsSaveData);

  FPlayerSavaState PlayerSaveState = CurrentSaveGame->PlayerSaveState;
  FComponentSaveState PComponentSaveState = *CurrentSaveGame->ComponentSaveStates.FindByPredicate(
      [PlayerSaveState](const FComponentSaveState& ComponentSaveState) {
        return ComponentSaveState.Id == PlayerSaveState.ActorComponentsMap["InventoryComponent"];
      });
  TArray<FObjectSaveState> PObjectSaveStates = CurrentSaveGame->ObjectSaveStates.FilterByPredicate(
      [PComponentSaveState](const FObjectSaveState& ObjectSaveState) {
        return PComponentSaveState.ComponentObjects.Contains(ObjectSaveState.Id);
      });
  LoadPlayer(PComponentSaveState, PObjectSaveStates);
}

auto ASaveManager::SaveAllSystems() -> TArray<FSystemSaveState> {
  TArray<FSystemSaveState> SystemSaveStates = {};
  SystemSaveStates.Reserve(SystemsToSave.Num());

  for (auto& System : SystemsToSave) {
    FSystemSaveState SystemSaveState = SaveSystemState(System.Value, System.Key);
    SystemSaveStates.Add(SystemSaveState);
  }

  return SystemSaveStates;
}

void ASaveManager::LoadAllSystems(TArray<FSystemSaveState> SystemSaveStates) {
  TArray<FName> CustomLoadSystems = {"MarketEconomy"};
  for (auto& System : SystemSaveStates) {
    if (CustomLoadSystems.Contains(System.Name)) continue;
    FMemoryReader MemReader(System.ByteData);
    FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
    Ar.ArIsSaveGame = true;
    SystemsToSave[System.Name]->Serialize(Ar);
  }

  FSystemSaveState MarketEconomySaveState = *SystemSaveStates.FindByPredicate(
      [](const FSystemSaveState& SystemSaveState) { return SystemSaveState.Name == "MarketEconomy"; });
  MarketEconomyCustomLoad(MarketEconomySaveState);
}

void ASaveManager::MarketEconomyCustomLoad(const FSystemSaveState& SystemSaveState) const {
  // To avoid saving everything, first use the data tables to create the default values.
  // Then copy the SaveGame values to the default values (by overwriting, then resetting the non SaveGame values).

  auto FormerPopEconDataArray = MarketEconomy->PopEconDataArray;
  auto FormerEconItems = MarketEconomy->EconItems;

  FMemoryReader MemReader(SystemSaveState.ByteData);
  FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
  Ar.ArIsSaveGame = true;
  MarketEconomy->Serialize(Ar);

  for (auto& PopEconData : MarketEconomy->PopEconDataArray) {
    auto FormerPopEconData = *FormerPopEconDataArray.FindByPredicate(
        [PopEconData](const auto& FormerPopEconData) { return FormerPopEconData.PopID == PopEconData.PopID; });

    SetStructNonSaveGameProperties(PopEconData.StaticStruct(), PopEconData, FormerPopEconData);
  }
  for (auto& EconItem : MarketEconomy->EconItems) {
    auto FormerEconItem =
        *FormerEconItems.FindByPredicate([EconItem](const FEconItem& Item) { return Item.ItemID == EconItem.ItemID; });

    SetStructNonSaveGameProperties(EconItem.StaticStruct(), EconItem, FormerEconItem);
  }

  for (int32 i = 0; i < MarketEconomy->PopEconDataArray.Num(); i++)
    check(MarketEconomy->CustomerPops[i].ID == MarketEconomy->PopEconDataArray[i].PopID);
}

auto ASaveManager::SaveLevels() -> FLevelsSaveData {
  // Saving only store for now.
  FLevelsSaveData LevelsSaveData;

  Store->SaveStoreLevelState();
  FLevelSaveState StoreLevelSaveState;
  StoreLevelSaveState.Name = "Store";
  for (auto& Pair : Store->StoreLevelState.ActorSaveMap) {
    StoreLevelSaveState.ActorIds.Add(Pair.Key);
    LevelsSaveData.ActorSaveMap.Add(Pair.Key, Pair.Value);
  }
  for (auto& Pair : Store->StoreLevelState.ComponentSaveMap) {
    StoreLevelSaveState.ActorComponentIds.Add(Pair.Key);
    LevelsSaveData.ComponentSaveMap.Add(Pair.Key, Pair.Value);
  }
  for (auto& o : Store->StoreLevelState.ObjectSaveStates) {
    StoreLevelSaveState.ComponentObjectIds.Add(o.Id);
    LevelsSaveData.ObjectSaveStates.Add(o);
  }
  LevelsSaveData.LevelSaveMap.Add(StoreLevelSaveState.Name, StoreLevelSaveState);

  return LevelsSaveData;
}

// ! Need to unload the level first or it won't overwrite that level's data.
void ASaveManager::LoadLevels(FLevelsSaveData LevelsSaveData) {
  FLevelSaveState StoreLevelSaveState = LevelsSaveData.LevelSaveMap["Store"];
  FStoreLevelState StoreLevelState;
  for (auto& Id : StoreLevelSaveState.ActorIds) StoreLevelState.ActorSaveMap.Add(Id, LevelsSaveData.ActorSaveMap[Id]);
  for (auto& Id : StoreLevelSaveState.ActorComponentIds)
    StoreLevelState.ComponentSaveMap.Add(Id, LevelsSaveData.ComponentSaveMap[Id]);
  for (auto& Id : StoreLevelSaveState.ComponentObjectIds)
    StoreLevelState.ObjectSaveStates.Add(*LevelsSaveData.ObjectSaveStates.FindByPredicate(
        [Id](const FObjectSaveState& ObjectSaveState) { return ObjectSaveState.Id == Id; }));

  Store->StoreLevelState.ActorSaveMap.Empty();
  Store->StoreLevelState.ComponentSaveMap.Empty();
  Store->StoreLevelState.ObjectSaveStates.Empty();
  Store->StoreLevelState = StoreLevelState;

  Store->LoadStoreLevelState();
}

auto ASaveManager::SavePlayer() -> TTuple<FPlayerSavaState, FComponentSaveState, TArray<FObjectSaveState>> {
  auto [ComponentSaveState, FObjectSaveStates] = SaveInventoryCSaveState(PlayerCharacter->PlayerInventoryComponent);
  FPlayerSavaState PlayerSaveState;
  PlayerSaveState.ActorComponentsMap.Add("InventoryComponent", ComponentSaveState.Id);

  return {PlayerSaveState, ComponentSaveState, FObjectSaveStates};
}
void ASaveManager::LoadPlayer(FComponentSaveState ComponentSaveState, TArray<FObjectSaveState> ObjectSaveStates) {
  LoadInventoryCSaveState(PlayerCharacter->PlayerInventoryComponent, ComponentSaveState, ObjectSaveStates);
}

auto ASaveManager::SaveInventoryCSaveState(UInventoryComponent* InventoryC) const
    -> TTuple<FComponentSaveState, TArray<FObjectSaveState>> {
  FComponentSaveState ComponentSaveState = SaveComponent(InventoryC, FGuid::NewGuid());

  TArray<FObjectSaveState> ObjectSaveStates;
  ObjectSaveStates.Reserve(InventoryC->ItemsArray.Num());
  for (UItemBase* Item : InventoryC->ItemsArray) {
    FObjectSaveState ObjectSaveState = SaveObject(Item, Item->UniqueItemID);
    ObjectSaveStates.Add(ObjectSaveState);
    ComponentSaveState.ComponentObjects.Add(Item->UniqueItemID);
  }

  return {ComponentSaveState, ObjectSaveStates};
}
void ASaveManager::LoadInventoryCSaveState(UInventoryComponent* InventoryC,
                                           FComponentSaveState ComponentSaveState,
                                           TArray<FObjectSaveState> ObjectSaveStates) const {
  FMemoryReader MemReader(ComponentSaveState.ByteData);
  FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
  Ar.ArIsSaveGame = true;
  InventoryC->Serialize(Ar);

  // TODO: Guarantee order.
  InventoryC->ItemsArray.Empty();
  InventoryC->ItemsArray.Reserve(ObjectSaveStates.Num());
  for (FObjectSaveState ObjectSaveState : ObjectSaveStates) {
    FMemoryReader OMemReader(ObjectSaveState.ByteData);
    FObjectAndNameAsStringProxyArchive OAr(OMemReader, true);
    OAr.ArIsSaveGame = true;

    UItemBase* Item = NewObject<UItemBase>(InventoryC, UItemBase::StaticClass());
    Item->Serialize(OAr);

    for (const auto& Id : Market->EligibleItemIds) {
      if (Id == Item->ItemID) {
        Item->SetItemFromBase(Market->AllItemsMap[Id]);
        break;
      }
    }

    InventoryC->ItemsArray.Add(Item);
  }
}

auto ASaveManager::SaveSystemState(AInfo* System, const FName& Name) const -> FSystemSaveState {
  FSystemSaveState SaveState;
  SaveState.Name = Name;

  FMemoryWriter MemWriter(SaveState.ByteData);
  FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
  Ar.ArIsSaveGame = true;
  System->Serialize(Ar);

  return SaveState;
}

auto ASaveManager::SaveActor(AActor* Actor, FGuid Id) const -> FActorSavaState {
  FActorSavaState SaveState;
  SaveState.Id = Id;

  FMemoryWriter MemWriter(SaveState.ByteData);
  FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
  Ar.ArIsSaveGame = true;
  Actor->Serialize(Ar);

  return SaveState;
}
auto ASaveManager::SaveComponent(UActorComponent* Component, FGuid Id) const -> FComponentSaveState {
  FComponentSaveState SaveState;
  SaveState.Id = Id;

  FMemoryWriter MemWriter(SaveState.ByteData);
  FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
  Ar.ArIsSaveGame = true;
  Component->Serialize(Ar);

  return SaveState;
}
auto ASaveManager::SaveObject(UObject* Object, FGuid Id) const -> FObjectSaveState {
  FObjectSaveState SaveState;
  SaveState.Id = Id;

  FMemoryWriter MemWriter(SaveState.ByteData);
  FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
  Ar.ArIsSaveGame = true;
  Object->Serialize(Ar);

  return SaveState;
}