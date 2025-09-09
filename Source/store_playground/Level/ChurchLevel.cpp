#include "ChurchLevel.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/Array.h"
#include "GameplayTagContainer.h"
#include "HAL/Platform.h"
#include "Logging/LogVerbosity.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/Guid.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Templates/Tuple.h"
#include "UObject/NameTypes.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/WorldObject/Level/NpcStoreSpawnPoint.h"
#include "store_playground/WorldObject/Level/PickupActor.h"
#include "store_playground/WorldObject/NPCStore.h"
#include "store_playground/WorldObject/MobileNPCStore.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Market/NpcStoreComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/WorldObject/Level/NpcSpawnPoint.h"
#include "store_playground/WorldObject/Npc.h"
#include "store_playground/Quest/QuestManager.h"
#include "store_playground/Quest/QuestComponent.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/WorldObject/Level/MiniGameSpawnPoint.h"
#include "store_playground/WorldObject/MiniGame.h"
#include "store_playground/Minigame/MiniGameManager.h"
#include "store_playground/Minigame/MiniGameStructs.h"
#include "store_playground/Minigame/MiniGameComponent.h"
#include "store_playground/Sprite/SimpleSpriteAnimComponent.h"
#include "store_playground/Player/PlayerCommand.h"
#include "PaperFlipbookComponent.h"
#include "PaperZDAnimationComponent.h"
#include "Components/WidgetComponent.h"

AChurchLevel::AChurchLevel() { PrimaryActorTick.bCanEverTick = false; }

void AChurchLevel::BeginPlay() {
  Super::BeginPlay();

  check(PickupActorClass);
}

void AChurchLevel::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AChurchLevel::EnterLevel() {}

void AChurchLevel::SaveLevelState() {
  LevelState.ActorSaveMap.Empty();
  LevelState.ComponentSaveMap.Empty();
  LevelState.ObjectSaveStates.Empty();
  LevelState.PersistentIds.Empty();

  TArray<APickupActor*> PickupActors = GetAllActorsOf<APickupActor>(GetWorld(), PickupActorClass);
  auto FilteredPickups =
      PickupActors.FilterByPredicate([](APickupActor* Pickup) { return Pickup->PickupComponent->bIsPicked; });
  for (APickupActor* Pickup : FilteredPickups) {
    FActorSavaState ActorSaveState = SaveManager->SaveActor(Pickup, Pickup->Id);

    auto PickupCSaveState = SaveManager->SaveComponent(Pickup->PickupComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("PickupComponent", PickupCSaveState.Id);

    LevelState.ActorSaveMap.Add(ActorSaveState.Id, ActorSaveState);
    LevelState.ComponentSaveMap.Add(PickupCSaveState.Id, PickupCSaveState);

    LevelState.PersistentIds.Add(ActorSaveState.Id);
    LevelState.PersistentIds.Add(PickupCSaveState.Id);
  }
}

void AChurchLevel::LoadLevelState() {
  // * Persistent actors.
  TArray<APickupActor*> PickupActors = GetAllActorsOf<APickupActor>(GetWorld(), PickupActorClass);
  for (APickupActor* Pickup : PickupActors) {
    if (!LevelState.ActorSaveMap.Contains(Pickup->Id)) continue;

    FActorSavaState ActorSaveState = LevelState.ActorSaveMap[Pickup->Id];
    auto PickupCSaveState = LevelState.ComponentSaveMap[ActorSaveState.ActorComponentsMap["PickupComponent"]];

    SaveManager->LoadActor(Pickup, ActorSaveState);
    SaveManager->LoadComponent(Pickup->PickupComponent, PickupCSaveState);

    Pickup->PickupComponent->DestroyPickup();
  }
}

void AChurchLevel::ResetDaysLevelState() {
  TMap<FGuid, FActorSavaState> PersistentActors = LevelState.ActorSaveMap.FilterByPredicate(
      [this](const TPair<FGuid, FActorSavaState>& Pair) { return LevelState.PersistentIds.Contains(Pair.Key); });
  TMap<FGuid, FComponentSaveState> PersistentComponents = LevelState.ComponentSaveMap.FilterByPredicate(
      [this](const TPair<FGuid, FComponentSaveState>& Pair) { return LevelState.PersistentIds.Contains(Pair.Key); });
  TArray<FObjectSaveState> PersistentObjects =
      LevelState.ObjectSaveStates.FilterByPredicate([this](const FObjectSaveState& ObjectSaveState) {
        return LevelState.PersistentIds.Contains(ObjectSaveState.Id);
      });

  LevelState.ActorSaveMap.Empty();
  LevelState.ComponentSaveMap.Empty();
  LevelState.ObjectSaveStates.Empty();

  LevelState.ActorSaveMap = PersistentActors;
  LevelState.ComponentSaveMap = PersistentComponents;
  LevelState.ObjectSaveStates = PersistentObjects;
}