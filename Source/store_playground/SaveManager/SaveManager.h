#pragma once

#include <functional>
#include "GameFramework/Info.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "SaveManager.generated.h"

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ASaveManager : public AInfo {
  GENERATED_BODY()

public:
  ASaveManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY()
  class APlayerZDCharacter* PlayerCharacter;
  UPROPERTY()
  TMap<FName, AInfo*> SystemsToSave;

  UPROPERTY()
  class AMarket* Market;
  UPROPERTY()
  class AMarketEconomy* MarketEconomy;
  UPROPERTY()
  class AStore* Store;
  UPROPERTY()
  class AUpgradeManager* UpgradeManager;
  UPROPERTY()
  class AGlobalDataManager* GlobalDataManager;
  UPROPERTY()
  class AGlobalStaticDataManager* GlobalStaticDataManager;

  UPROPERTY()
  class UMySaveGame* CurrentSaveGame;

  void CreateNewSaveGame();

  void SaveCurrentSlotToDisk();
  // Need to be separated to load store level info.
  void LoadSystemsFromDisk();
  void LoadLevelsAndPlayerFromDisk();

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

  void LoadActor(AActor* Actor, FActorSavaState SaveState) const;
  void LoadComponent(UActorComponent* Component, FComponentSaveState SaveState) const;
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