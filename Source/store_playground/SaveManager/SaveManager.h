#pragma once

#include <functional>
#include "Components/MeshComponent.h"
#include "GameFramework/Info.h"
#include "store_playground/Framework/StorePGGameInstance.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "SaveManager.generated.h"

USTRUCT()
struct FSaveManagerParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FString SettingsSaveName;
  UPROPERTY(EditAnywhere)
  FString SaveSlotListSaveName;
  UPROPERTY(EditAnywhere)
  int32 SaveSlotCount;
  UPROPERTY(EditAnywhere)
  FString SaveSlotNamePrefix;
  UPROPERTY(EditAnywhere)
  FString AutoSaveSlotName;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ASaveManager : public AInfo {
  GENERATED_BODY()

public:
  ASaveManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY()
  const class ALevelManager* LevelManager;
  UPROPERTY()
  const class AMarket* Market;
  UPROPERTY()
  const class AUpgradeManager* UpgradeManager;
  UPROPERTY()
  const class AGlobalDataManager* GlobalDataManager;
  UPROPERTY()
  const class AGlobalStaticDataManager* GlobalStaticDataManager;
  UPROPERTY()
  const class ADayManager* DayManager;
  UPROPERTY()
  const class AStorePhaseManager* StorePhaseManager;

  UPROPERTY(EditAnywhere)
  FSaveManagerParams SaveManagerParams;

  UPROPERTY()
  class APlayerZDCharacter* PlayerCharacter;
  UPROPERTY()
  TMap<FName, AInfo*> SystemsToSave;

  UPROPERTY()
  class AMarketLevel* MarketLevel;
  UPROPERTY()
  class AChurchLevel* ChurchLevel;
  UPROPERTY()
  class AStore* Store;
  UPROPERTY()
  class AMarketEconomy* MarketEconomy;

  UPROPERTY()
  class USettingsSaveGame* SettingsSaveGame;
  UPROPERTY()
  class USaveSlotListSaveGame* SaveSlotListSaveGame;
  UPROPERTY()
  class UMySaveGame* CurrentSaveGame;
  UPROPERTY()
  class UMySaveGame* AutoSaveGame;

  void SaveSettingsToDisk(const FGameSettings NewSettings,
                          const FSavedSoundSettings SoundSettings,
                          const FAdvGraphicsSettings AdvGraphicsSettings);
  auto LoadSettingsFromDisk() -> class USettingsSaveGame*;

  void LoadSaveGameSlots();

  auto CanSave() const -> bool;  // * For UI.
  void CreateNewSaveGame(int32 SlotIndex);
  void SaveCurrentSlotToDisk();
  // Systems and Levels need to be separated to load store level info.
  void LoadSystemsFromDisk(int32 SlotIndex);
  void LoadLevelsAndPlayerFromDisk();
  void DeleteSaveGame(int32 SlotIndex);

  void AutoSave();
  void LoadAutoSave();

  auto SaveAllSystems() -> TArray<FSystemSaveState>;
  void LoadAllSystems(TArray<FSystemSaveState> SystemSaveStates);
  void MarketEconomyCustomLoad(const FSystemSaveState& SystemSaveState);
  auto SaveLevels() -> FLevelsSaveData;
  void LoadLevels(FLevelsSaveData LevelsSaveData);

  // Only inventory component for now.
  auto SavePlayer() -> TTuple<FPlayerSavaState, TArray<FComponentSaveState>, TArray<FObjectSaveState>>;
  void LoadPlayer(FPlayerSavaState PlayerSaveState,
                  TArray<FComponentSaveState> ComponentSaveState,
                  TArray<FObjectSaveState> ObjectSaveStates);

  void ApplyLoadedUpgradeEffects();  // * Apply ChangeData upgrade effects to the data manager.

  auto SaveInventoryCSaveState(class UInventoryComponent* InventoryC) const
      -> TTuple<FComponentSaveState, TArray<FObjectSaveState>>;
  void LoadInventoryCSaveState(class UInventoryComponent* InventoryC,
                               FComponentSaveState ComponentSaveState,
                               TArray<FObjectSaveState> ObjectSaveStates) const;

  auto SaveSystemState(class AInfo* System, const FName& Name) const -> FSystemSaveState;
  auto SaveActor(AActor* Actor, FGuid Id) const -> FActorSavaState;
  auto SaveComponent(UActorComponent* Component, FGuid Id) const -> FComponentSaveState;
  auto SaveObject(UObject* Object, FGuid Id) const -> FObjectSaveState;
  auto SaveMesh(UMeshComponent* Mesh, FGuid Id) const -> FComponentSaveState;

  void LoadActor(AActor* Actor, FActorSavaState SaveState) const;
  void LoadComponent(UActorComponent* Component, FComponentSaveState SaveState) const;
  void LoadMesh(UMeshComponent* Mesh, FComponentSaveState SaveState) const;
};

// template <typename T>
// concept UStruct = requires(T) { typename T::StaticStruct; };
// Writing
// MyPawn->Serialize(Ar);
// TArray<UActorComponent*> Components = MyPawn->GetAllComponents();
// int32 Num = Components.Num();
// Ar << Num;
// for (auto Comp : Components)
// {
//     FString Name = Comp->GetName();
//     Ar << Name;
//     Comp->Serialize(Ar);
// }

// // Reading :
// MyPawn->Serialize(Ar);
// int32 Num;
// Ar << Num;
// for (int32 i=0; i<Num; i++)
// {
//     FString Name;
//     Ar << Name;
//     UActorComponent* Comp = MyPawn->FindComponentByName(Name);
//     Comp->Serialize(Ar);
// }