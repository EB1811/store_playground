#include "SaveManager.h"
#include "Logging/LogVerbosity.h"
#include "Misc/AssertionMacros.h"
#include "SaveStructs.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Level/MarketLevel.h"
#include "store_playground/SaveManager/SettingsSaveGame.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Level/LevelManager.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/Quest/QuestManager.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "store_playground/SaveManager/MySaveGame.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/Player/PlayerZDCharacter.h"
#include "store_playground/Tags/TagsComponent.h"
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

ASaveManager::ASaveManager() {
  PrimaryActorTick.bCanEverTick = false;

  SaveManagerParams.SettingsSaveName = "Settings";
  SaveManagerParams.SaveSlotListSaveName = "SaveSlotList";
  SaveManagerParams.SaveSlotCount = 4;
  SaveManagerParams.SaveSlotNamePrefix = "SaveSlot_";
  CurrentSaveGame = nullptr;
  AutoSaveGame = nullptr;
}

void ASaveManager::BeginPlay() { Super::BeginPlay(); }

void ASaveManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ASaveManager::SaveSettingsToDisk(const FGameSettings NewSettings,
                                      const FSavedSoundSettings SoundSettings,
                                      const FAdvGraphicsSettings AdvGraphicsSettings) {
  if (!UGameplayStatics::DoesSaveGameExist(SaveManagerParams.SettingsSaveName, 0)) LoadSettingsFromDisk();
  check(SettingsSaveGame);

  SettingsSaveGame->GameSettings = NewSettings;
  SettingsSaveGame->SoundSettings = SoundSettings;
  SettingsSaveGame->AdvGraphicsSettings = AdvGraphicsSettings;
  UGameplayStatics::SaveGameToSlot(SettingsSaveGame, SaveManagerParams.SettingsSaveName, 0);
}
auto ASaveManager::LoadSettingsFromDisk() -> USettingsSaveGame* {
  if (!UGameplayStatics::DoesSaveGameExist(SaveManagerParams.SettingsSaveName, 0)) {
    SettingsSaveGame =
        Cast<USettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(USettingsSaveGame::StaticClass()));
    check(SettingsSaveGame);

    SettingsSaveGame->GameSettings = {
        .Difficulty = EGameDifficulty::Normal,
        .bShowTutorials = true,
    };
    SettingsSaveGame->SoundSettings = {};
    SettingsSaveGame->AdvGraphicsSettings = {.AntiAliasingMethod = 0,
                                             .GlobalIlluminationMethod = 0,
                                             .ReflectionMethod = 0,
                                             .bDepthOfField = true,
                                             .bBloom = true,
                                             .bDLSSFrameGeneration = false};
    UGameplayStatics::SaveGameToSlot(SettingsSaveGame, SaveManagerParams.SettingsSaveName, 0);
    UE_LOG(LogTemp, Warning, TEXT("SaveManager: Created new settings save game."));

    return SettingsSaveGame;
  }

  SettingsSaveGame = Cast<USettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveManagerParams.SettingsSaveName, 0));
  check(SettingsSaveGame);

  return SettingsSaveGame;
}

void ASaveManager::LoadSaveGameSlots() {
  if (!UGameplayStatics::DoesSaveGameExist(SaveManagerParams.SaveSlotListSaveName, 0)) {
    SaveSlotListSaveGame =
        Cast<USaveSlotListSaveGame>(UGameplayStatics::CreateSaveGameObject(USaveSlotListSaveGame::StaticClass()));
    check(SaveSlotListSaveGame);

    SaveSlotListSaveGame->SaveSlotList = {};
    for (int32 i = 0; i < SaveManagerParams.SaveSlotCount; i++) {
      FSaveSlotData Data;
      Data.SlotName = SaveManagerParams.SaveSlotNamePrefix + FString::FromInt(i + 1);
      Data.bIsPopulated = false;
      Data.LastModified = FDateTime::Now();
      Data.CurrentDay = 0;
      Data.StoreMoney = 0.0f;
      SaveSlotListSaveGame->SaveSlotList.Add(Data);
    }
    SaveSlotListSaveGame->MostRecentSaveSlotIndex = 0;

    UGameplayStatics::SaveGameToSlot(SaveSlotListSaveGame, SaveManagerParams.SaveSlotListSaveName, 0);
    UE_LOG(LogTemp, Warning, TEXT("SaveManager: Created new save slot list."));

    return;
  }

  SaveSlotListSaveGame =
      Cast<USaveSlotListSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveManagerParams.SaveSlotListSaveName, 0));
  check(SaveSlotListSaveGame);
}

auto ASaveManager::CanSave() const -> bool {
  check(StorePhaseManager);
  return StorePhaseManager->StorePhaseState != EStorePhaseState::None &&
         StorePhaseManager->StorePhaseState != EStorePhaseState::MorningBuildMode &&
         StorePhaseManager->StorePhaseState != EStorePhaseState::ShopOpen;
}

void ASaveManager::CreateNewSaveGame(int32 SlotIndex) {
  check(SaveSlotListSaveGame);
  check(SlotIndex < SaveManagerParams.SaveSlotCount);
  check(Store && DayManager);
  check(CanSave());

  CurrentSaveGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
  check(CurrentSaveGame);

  CurrentSaveGame->Initialize(SaveManagerParams.SaveSlotNamePrefix + FString::FromInt(SlotIndex), SlotIndex);
  SaveCurrentSlotToDisk();

  if (SlotIndex >= SaveSlotListSaveGame->SaveSlotList.Num())
    for (int32 i = SaveSlotListSaveGame->SaveSlotList.Num(); i <= SlotIndex; i++)
      SaveSlotListSaveGame->SaveSlotList.Add({});
  SaveSlotListSaveGame->SaveSlotList[SlotIndex].bIsPopulated = true;
  SaveSlotListSaveGame->SaveSlotList[SlotIndex].SlotName = CurrentSaveGame->SlotName;
  SaveSlotListSaveGame->SaveSlotList[SlotIndex].LastModified = FDateTime::Now();
  SaveSlotListSaveGame->SaveSlotList[SlotIndex].CurrentDay = DayManager->CurrentDay;
  SaveSlotListSaveGame->SaveSlotList[SlotIndex].StoreMoney = Store->Money;
  SaveSlotListSaveGame->MostRecentSaveSlotIndex = SlotIndex;

  UGameplayStatics::SaveGameToSlot(SaveSlotListSaveGame, SaveManagerParams.SaveSlotListSaveName, 0);

  UE_LOG(LogTemp, Warning, TEXT("SaveManager: Created new save game."));
}

void ASaveManager::SaveCurrentSlotToDisk() {
  check(CurrentSaveGame);

  CurrentSaveGame->SystemSaveStates = SaveAllSystems();

  FLevelsSaveData LevelsSaveData = SaveLevels();
  for (auto& Pair : LevelsSaveData.LevelSaveMap) CurrentSaveGame->LevelSaveStates.Add(Pair.Value);
  for (auto& Pair : LevelsSaveData.ActorSaveMap) CurrentSaveGame->ActorSaveStates.Add(Pair.Value);
  for (auto& Pair : LevelsSaveData.ComponentSaveMap) CurrentSaveGame->ComponentSaveStates.Add(Pair.Value);
  CurrentSaveGame->ObjectSaveStates.Append(LevelsSaveData.ObjectSaveStates);

  auto [PlayerSaveState, PComponentSaveStates, PObjectSaveStates] = SavePlayer();
  CurrentSaveGame->PlayerSaveState = PlayerSaveState;
  CurrentSaveGame->ComponentSaveStates.Append(PComponentSaveStates);
  CurrentSaveGame->ObjectSaveStates.Append(PObjectSaveStates);

  UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSaveGame->SlotName, CurrentSaveGame->SlotIndex);
}

void ASaveManager::LoadSystemsFromDisk(int32 SlotIndex) {
  check(SaveSlotListSaveGame && SlotIndex < SaveManagerParams.SaveSlotCount);

  if (SlotIndex >= 0) {
    FString SlotName = SaveSlotListSaveGame->SaveSlotList[SlotIndex].SlotName;
    CurrentSaveGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
    check(CurrentSaveGame);

    LoadAllSystems(CurrentSaveGame->SystemSaveStates);
  } else if (SlotIndex == -1) {  // Auto-save
    AutoSaveGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("AutoSave", -1));
    check(AutoSaveGame);

    LoadAllSystems(AutoSaveGame->SystemSaveStates);
  } else {
    checkNoEntry();
  }

  ApplyLoadedUpgradeEffects();
}
void ASaveManager::LoadLevelsAndPlayerFromDisk() {
  auto SaveGame = CurrentSaveGame ? CurrentSaveGame : AutoSaveGame;
  check(SaveGame);

  FLevelsSaveData LevelsSaveData;
  for (auto& Pair : SaveGame->LevelSaveStates) LevelsSaveData.LevelSaveMap.Add(Pair.Name, Pair);
  for (auto& Pair : SaveGame->ActorSaveStates) LevelsSaveData.ActorSaveMap.Add(Pair.Id, Pair);
  for (auto& Pair : SaveGame->ComponentSaveStates) LevelsSaveData.ComponentSaveMap.Add(Pair.Id, Pair);
  LevelsSaveData.ObjectSaveStates = SaveGame->ObjectSaveStates;
  LoadLevels(LevelsSaveData);

  FPlayerSavaState PlayerSaveState = SaveGame->PlayerSaveState;
  TArray<FComponentSaveState> PComponentSaveStates =
      SaveGame->ComponentSaveStates.FilterByPredicate([PlayerSaveState](const FComponentSaveState& ComponentSaveState) {
        return ComponentSaveState.Id == PlayerSaveState.ActorComponentsMap["InventoryComponent"] ||
               ComponentSaveState.Id == PlayerSaveState.ActorComponentsMap["TagsComponent"];
      });
  TArray<FObjectSaveState> PObjectSaveStates =
      SaveGame->ObjectSaveStates.FilterByPredicate([PComponentSaveStates](const FObjectSaveState& ObjectSaveState) {
        return PComponentSaveStates.ContainsByPredicate(
            [ObjectSaveState](const FComponentSaveState& ComponentSaveState) {
              return ComponentSaveState.ComponentObjects.Contains(ObjectSaveState.Id);
            });
      });
  LoadPlayer(PlayerSaveState, PComponentSaveStates, PObjectSaveStates);
}
void ASaveManager::DeleteSaveGame(int32 SlotIndex) {
  check(SaveSlotListSaveGame && SlotIndex < SaveManagerParams.SaveSlotCount);

  FString SlotName = SaveSlotListSaveGame->SaveSlotList[SlotIndex].SlotName;
  UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);

  SaveSlotListSaveGame->SaveSlotList[SlotIndex].bIsPopulated = false;
  if (SaveSlotListSaveGame->MostRecentSaveSlotIndex == SlotIndex) SaveSlotListSaveGame->MostRecentSaveSlotIndex = -1;

  UGameplayStatics::SaveGameToSlot(SaveSlotListSaveGame, SaveManagerParams.SaveSlotListSaveName, 0);
  UE_LOG(LogTemp, Warning, TEXT("SaveManager: Deleted save game at slot %d."), SlotIndex);
}

void ASaveManager::AutoSave() {
  if (!CanSave()) return;

  if (!AutoSaveGame) {
    AutoSaveGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
    check(AutoSaveGame);

    AutoSaveGame->Initialize("AutoSave", -1);
  }

  AutoSaveGame->SystemSaveStates = SaveAllSystems();

  FLevelsSaveData LevelsSaveData = SaveLevels();
  for (auto& Pair : LevelsSaveData.LevelSaveMap) AutoSaveGame->LevelSaveStates.Add(Pair.Value);
  for (auto& Pair : LevelsSaveData.ActorSaveMap) AutoSaveGame->ActorSaveStates.Add(Pair.Value);
  for (auto& Pair : LevelsSaveData.ComponentSaveMap) AutoSaveGame->ComponentSaveStates.Add(Pair.Value);
  AutoSaveGame->ObjectSaveStates.Append(LevelsSaveData.ObjectSaveStates);

  auto [PlayerSaveState, PComponentSaveStates, PObjectSaveStates] = SavePlayer();
  AutoSaveGame->PlayerSaveState = PlayerSaveState;
  AutoSaveGame->ComponentSaveStates.Append(PComponentSaveStates);
  AutoSaveGame->ObjectSaveStates.Append(PObjectSaveStates);

  WithLog(
      [this]() {
        return UGameplayStatics::SaveGameToSlot(AutoSaveGame, AutoSaveGame->SlotName, AutoSaveGame->SlotIndex);
      },
      "AutoSave: Auto-saved successfully.", "AutoSave: Auto-save failed.");

  SaveSlotListSaveGame->MostRecentSaveSlotIndex = -1;
  SaveSlotListSaveGame->bHasAutoSave = true;
  UGameplayStatics::SaveGameToSlot(SaveSlotListSaveGame, SaveManagerParams.SaveSlotListSaveName, 0);
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

void ASaveManager::MarketEconomyCustomLoad(const FSystemSaveState& SystemSaveState) {
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
  FLevelsSaveData LevelsSaveData;

  switch (LevelManager->CurrentLevel) {
    case ELevel::Store: Store->SaveStoreLevelState(); break;
    case ELevel::Market: MarketLevel->SaveLevelState(); break;
    default: break;
  }

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

  FLevelSaveState MarketLevelSaveState;
  MarketLevelSaveState.Name = "MarketLevel";
  for (auto& Pair : MarketLevel->LevelState.ActorSaveMap) {
    MarketLevelSaveState.ActorIds.Add(Pair.Key);
    LevelsSaveData.ActorSaveMap.Add(Pair.Key, Pair.Value);
  }
  for (auto& Pair : MarketLevel->LevelState.ComponentSaveMap) {
    MarketLevelSaveState.ActorComponentIds.Add(Pair.Key);
    LevelsSaveData.ComponentSaveMap.Add(Pair.Key, Pair.Value);
  }
  for (auto& o : MarketLevel->LevelState.ObjectSaveStates) {
    MarketLevelSaveState.ComponentObjectIds.Add(o.Id);
    LevelsSaveData.ObjectSaveStates.Add(o);
  }
  LevelsSaveData.LevelSaveMap.Add(MarketLevelSaveState.Name, MarketLevelSaveState);

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

  if (LevelsSaveData.LevelSaveMap.Contains("MarketLevel")) {
    FLevelSaveState MarketLevelSaveState = LevelsSaveData.LevelSaveMap["MarketLevel"];
    FMarketLevelState MarketLevelState;
    for (auto& Id : MarketLevelSaveState.ActorIds)
      MarketLevelState.ActorSaveMap.Add(Id, LevelsSaveData.ActorSaveMap[Id]);
    for (auto& Id : MarketLevelSaveState.ActorComponentIds)
      MarketLevelState.ComponentSaveMap.Add(Id, LevelsSaveData.ComponentSaveMap[Id]);
    for (auto& Id : MarketLevelSaveState.ComponentObjectIds)
      MarketLevelState.ObjectSaveStates.Add(*LevelsSaveData.ObjectSaveStates.FindByPredicate(
          [Id](const FObjectSaveState& ObjectSaveState) { return ObjectSaveState.Id == Id; }));

    MarketLevel->LevelState.ActorSaveMap.Empty();
    MarketLevel->LevelState.ComponentSaveMap.Empty();
    MarketLevel->LevelState.ObjectSaveStates.Empty();
    MarketLevel->LevelState = MarketLevelState;

    // MarketLevel->LoadLevelState();
  }
}

auto ASaveManager::SavePlayer() -> TTuple<FPlayerSavaState, TArray<FComponentSaveState>, TArray<FObjectSaveState>> {
  FPlayerSavaState PlayerSaveState;
  TArray<FComponentSaveState> ComponentSaveStates;

  auto [InvComponentSaveState, FObjectSaveStates] = SaveInventoryCSaveState(PlayerCharacter->PlayerInventoryComponent);
  PlayerSaveState.ActorComponentsMap.Add("InventoryComponent", InvComponentSaveState.Id);
  ComponentSaveStates.Add(InvComponentSaveState);

  FComponentSaveState TagComponentSaveState = SaveComponent(PlayerCharacter->PlayerTagsComponent, FGuid::NewGuid());
  PlayerSaveState.ActorComponentsMap.Add("TagsComponent", TagComponentSaveState.Id);
  ComponentSaveStates.Add(TagComponentSaveState);

  return {PlayerSaveState, ComponentSaveStates, FObjectSaveStates};
}
void ASaveManager::LoadPlayer(FPlayerSavaState PlayerSaveState,
                              TArray<FComponentSaveState> ComponentSaveStates,
                              TArray<FObjectSaveState> ObjectSaveStates) {
  FComponentSaveState PInventorySaveState =
      *ComponentSaveStates.FindByPredicate([PlayerSaveState](const FComponentSaveState& ComponentSaveState) {
        return ComponentSaveState.Id == PlayerSaveState.ActorComponentsMap["InventoryComponent"];
      });
  TArray<FObjectSaveState> InvObjectSaveStates =
      ObjectSaveStates.FilterByPredicate([PInventorySaveState](const FObjectSaveState& ObjectSaveState) {
        return PInventorySaveState.ComponentObjects.Contains(ObjectSaveState.Id);
      });

  LoadInventoryCSaveState(PlayerCharacter->PlayerInventoryComponent, PInventorySaveState, InvObjectSaveStates);
  LoadComponent(PlayerCharacter->PlayerTagsComponent,
                *ComponentSaveStates.FindByPredicate([PlayerSaveState](const FComponentSaveState& ComponentSaveState) {
                  return ComponentSaveState.Id == PlayerSaveState.ActorComponentsMap["TagsComponent"];
                }));
}

void ASaveManager::ApplyLoadedUpgradeEffects() {
  TArray<FUpgradeEffect> Effects = GlobalStaticDataManager->GetUpgradeEffectsByIds(UpgradeManager->ActiveEffectIDs);
  TArray<FUpgradeEffect> ChangeDataEffects = Effects.FilterByPredicate(
      [](const FUpgradeEffect& Effect) { return Effect.EffectType == EUpgradeEffectType::ChangeData; });

  FUpgradeable Upgradeable = GlobalDataManager->Upgradeable;
  for (const FUpgradeEffect& Effect : ChangeDataEffects) {
    check(!Effect.RelevantName.IsNone() && Effect.RelevantValues.Num() > 0);

    Upgradeable.ChangeData(Effect.RelevantName, Effect.RelevantIDs, Effect.RelevantValues);
  }
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

  InventoryC->ItemsArray.Empty();
  InventoryC->ItemsArray.Reserve(ObjectSaveStates.Num());
  for (FObjectSaveState ObjectSaveState : ObjectSaveStates) {
    FMemoryReader OMemReader(ObjectSaveState.ByteData);
    FObjectAndNameAsStringProxyArchive OAr(OMemReader, true);
    OAr.ArIsSaveGame = true;

    UItemBase* Item = NewObject<UItemBase>(InventoryC, UItemBase::StaticClass());
    Item->Serialize(OAr);
    Item->UniqueItemID = FGuid::NewGuid();

    for (const auto& MappedItem : Market->AllItemsMap) {
      if (MappedItem.Key == Item->ItemID) {
        Item->SetItemFromBase(Market->AllItemsMap[MappedItem.Key]);
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
auto ASaveManager::SaveMesh(UMeshComponent* Mesh, FGuid Id) const -> FComponentSaveState {
  FComponentSaveState SaveState;
  SaveState.Id = Id;

  FMemoryWriter MemWriter(SaveState.ByteData);
  FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
  Ar.ArIsSaveGame = false;
  Mesh->Serialize(Ar);

  return SaveState;
}
void ASaveManager::LoadActor(AActor* Actor, FActorSavaState SaveState) const {
  FMemoryReader MemReader(SaveState.ByteData);
  FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
  Ar.ArIsSaveGame = true;
  Actor->Serialize(Ar);
}
void ASaveManager::LoadComponent(UActorComponent* Component, FComponentSaveState SaveState) const {
  FMemoryReader MemReader(SaveState.ByteData);
  FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
  Ar.ArIsSaveGame = true;
  Component->Serialize(Ar);
}
void ASaveManager::LoadMesh(UMeshComponent* Mesh, FComponentSaveState SaveState) const {
  FMemoryReader MemReader(SaveState.ByteData);
  FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
  Ar.ArIsSaveGame = false;
  Mesh->Serialize(Ar);

  // ! Need to force update its static mesh.
  Mesh->SetVisibility(false);
  Mesh->SetVisibility(true);
}