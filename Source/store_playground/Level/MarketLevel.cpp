#include "MarketLevel.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/Array.h"
#include "HAL/Platform.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/Guid.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UObject/NameTypes.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/WorldObject/Level/NpcStoreSpawnPoint.h"
#include "store_playground/WorldObject/NPCStore.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Market/Market.h"
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
#include "store_playground/Player/PlayerCommand.h"

AMarketLevel::AMarketLevel() { PrimaryActorTick.bCanEverTick = false; }

void AMarketLevel::BeginPlay() {
  Super::BeginPlay();

  check(NpcStoreSpawnPointClass && NPCStoreClass && NpcSpawnPointClass && NpcClass && MiniGameSpawnPointClass &&
        MiniGameClass);
}

void AMarketLevel::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AMarketLevel::EnterLevel() {
  if (FMath::FRand() * 100 >= LevelParams.PlayerMiscDialogueChance) {
    auto MiscDialogues =
        GlobalStaticDataManager->GetRandomPlayerMiscDialogue({"Dialogue.Idle", "Dialogue.Level.Market"});
    PlayerCommand->CommandDialogue(MiscDialogues);
  }
}

void AMarketLevel::TickDaysTimedVars() {
  TArray<FName> NpcsToRemove;
  for (auto& Pair : RecentlySpawnedUniqueNpcsMap)
    if (Pair.Value <= 1) NpcsToRemove.Add(Pair.Key);
    else Pair.Value--;

  for (const auto& NpcId : NpcsToRemove) RecentlySpawnedUniqueNpcsMap.Remove(NpcId);
}

void AMarketLevel::SaveLevelState() {
  LevelState.NpcStoreSaveMap.Empty();
  LevelState.ActorSaveMap.Empty();
  LevelState.ComponentSaveMap.Empty();
  LevelState.ObjectSaveStates.Empty();

  TArray<ANPCStore*> FoundStores = GetAllActorsOf<ANPCStore>(GetWorld(), NPCStoreClass);
  for (ANPCStore* Store : FoundStores)
    LevelState.NpcStoreSaveMap.Add(Store->NpcStoreId, {
                                                          Store->NpcStoreId,
                                                          Store->DialogueComponent->DialogueArray,
                                                          Store->InventoryComponent->ItemsArray,
                                                          Store->NpcStoreComponent->NpcStoreType,
                                                      });

  TArray<ANpc*> Npcs = GetAllActorsOf<ANpc>(GetWorld(), NpcClass);
  for (auto* Npc : Npcs) {
    FActorSavaState ActorSaveState = SaveManager->SaveActor(Npc, Npc->SpawnPointId);  // Using SpawnPoint's id.

    auto InteractionCSaveState = SaveManager->SaveComponent(Npc->InteractionComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("InteractionComponent", InteractionCSaveState.Id);
    auto DialogueCSaveState = SaveManager->SaveComponent(Npc->DialogueComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("DialogueComponent", DialogueCSaveState.Id);
    auto QuestCSaveState = SaveManager->SaveComponent(Npc->QuestComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("QuestComponent", QuestCSaveState.Id);

    LevelState.ActorSaveMap.Add(ActorSaveState.Id, ActorSaveState);
    LevelState.ComponentSaveMap.Add(InteractionCSaveState.Id, InteractionCSaveState);
    LevelState.ComponentSaveMap.Add(DialogueCSaveState.Id, DialogueCSaveState);
    LevelState.ComponentSaveMap.Add(QuestCSaveState.Id, QuestCSaveState);
  }

  TArray<AMiniGame*> MiniGameActors = GetAllActorsOf<AMiniGame>(GetWorld(), MiniGameClass);
  for (AMiniGame* MiniGame : MiniGameActors) {
    FActorSavaState ActorSaveState =
        SaveManager->SaveActor(MiniGame, MiniGame->SpawnPointId);  // Using SpawnPoint's id.

    auto InteractionCSaveState = SaveManager->SaveComponent(MiniGame->InteractionComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("InteractionComponent", InteractionCSaveState.Id);
    auto DialogueCSaveState = SaveManager->SaveComponent(MiniGame->DialogueComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("DialogueComponent", DialogueCSaveState.Id);
    auto MiniGameCSaveState = SaveManager->SaveComponent(MiniGame->MiniGameComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("MiniGameComponent", MiniGameCSaveState.Id);

    LevelState.ActorSaveMap.Add(ActorSaveState.Id, ActorSaveState);
    LevelState.ComponentSaveMap.Add(InteractionCSaveState.Id, InteractionCSaveState);
    LevelState.ComponentSaveMap.Add(DialogueCSaveState.Id, DialogueCSaveState);
    LevelState.ComponentSaveMap.Add(MiniGameCSaveState.Id, MiniGameCSaveState);
  }
}

void AMarketLevel::LoadLevelState(bool bIsWeekend) {
  if (LevelState.NpcStoreSaveMap.Num() <= 0) {
    InitNPCStores(bIsWeekend);
    InitMarketNpcs(bIsWeekend);
    InitMiniGames(bIsWeekend);
    return;
  }

  TArray<ANPCStore*> FoundStores = GetAllActorsOf<ANPCStore>(GetWorld(), NPCStoreClass);
  for (ANPCStore* Store : FoundStores) {
    check(LevelState.NpcStoreSaveMap.Contains(Store->NpcStoreId));

    FNpcStoreSaveState SaveState = LevelState.NpcStoreSaveMap[Store->NpcStoreId];
    Store->NpcStoreId = SaveState.Id;
    Store->DialogueComponent->DialogueArray = SaveState.DialogueArray;
    Store->InventoryComponent->ItemsArray = SaveState.ItemsArray;
    Store->NpcStoreComponent->NpcStoreType = SaveState.NpcStoreType;
  }

  TArray<ANpcSpawnPoint*> SpawnPoints = GetAllActorsOf<ANpcSpawnPoint>(GetWorld(), NpcSpawnPointClass);
  check(SpawnPoints.Num() > 0);

  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.OverrideLevel = SpawnPoints[0]->GetLevel();
  SpawnParams.bNoFail = true;
  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

  for (ANpcSpawnPoint* SpawnPoint : SpawnPoints) {
    if (!LevelState.ActorSaveMap.Contains(SpawnPoint->Id)) continue;
    FActorSavaState NpcSaveState = LevelState.ActorSaveMap[SpawnPoint->Id];
    auto InteractionCSaveState = LevelState.ComponentSaveMap[NpcSaveState.ActorComponentsMap["InteractionComponent"]];
    auto DialogueCSaveState = LevelState.ComponentSaveMap[NpcSaveState.ActorComponentsMap["DialogueComponent"]];
    auto QuestCSaveState = LevelState.ComponentSaveMap[NpcSaveState.ActorComponentsMap["QuestComponent"]];

    ANpc* SpawnedNpc = GetWorld()->SpawnActor<ANpc>(NpcClass, SpawnPoint->GetActorLocation(),
                                                    SpawnPoint->GetActorRotation(), SpawnParams);
    SaveManager->LoadActor(SpawnedNpc, NpcSaveState);
    SaveManager->LoadComponent(SpawnedNpc->InteractionComponent, InteractionCSaveState);
    SaveManager->LoadComponent(SpawnedNpc->DialogueComponent, DialogueCSaveState);
    SaveManager->LoadComponent(SpawnedNpc->QuestComponent, QuestCSaveState);
  }

  TArray<AMiniGameSpawnPoint*> MiniGameSpawnPoints =
      GetAllActorsOf<AMiniGameSpawnPoint>(GetWorld(), MiniGameSpawnPointClass);
  check(MiniGameSpawnPoints.Num() > 0);

  for (auto* SpawnPoint : MiniGameSpawnPoints) {
    if (!LevelState.ActorSaveMap.Contains(SpawnPoint->Id)) continue;

    FActorSavaState MiniGameSaveState = LevelState.ActorSaveMap[SpawnPoint->Id];
    auto InteractionCSaveState =
        LevelState.ComponentSaveMap[MiniGameSaveState.ActorComponentsMap["InteractionComponent"]];
    auto DialogueCSaveState = LevelState.ComponentSaveMap[MiniGameSaveState.ActorComponentsMap["DialogueComponent"]];
    auto MiniGameCSaveState = LevelState.ComponentSaveMap[MiniGameSaveState.ActorComponentsMap["MiniGameComponent"]];

    AMiniGame* SpawnedMiniGame = GetWorld()->SpawnActor<AMiniGame>(MiniGameClass, SpawnPoint->GetActorLocation(),
                                                                   SpawnPoint->GetActorRotation(), SpawnParams);
    SaveManager->LoadActor(SpawnedMiniGame, MiniGameSaveState);
    SaveManager->LoadComponent(SpawnedMiniGame->InteractionComponent, InteractionCSaveState);
    SaveManager->LoadComponent(SpawnedMiniGame->DialogueComponent, DialogueCSaveState);
    SaveManager->LoadComponent(SpawnedMiniGame->MiniGameComponent, MiniGameCSaveState);
  }
}

void AMarketLevel::ResetLevelState() {
  LevelState.NpcStoreSaveMap.Empty();
  LevelState.ActorSaveMap.Empty();
  LevelState.ComponentSaveMap.Empty();
  LevelState.ObjectSaveStates.Empty();
}

void AMarketLevel::InitNPCStores(bool bIsWeekend) {
  check(GlobalDataManager && Market);

  TArray<ANpcStoreSpawnPoint*> SpawnPoints = GetAllActorsOf<ANpcStoreSpawnPoint>(GetWorld(), NpcStoreSpawnPointClass);
  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.bNoFail = true;
  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
  for (auto* SpawnPoint : SpawnPoints) {
    if (FMath::FRand() * 100 >= SpawnPoint->SpawnChance * (bIsWeekend ? LevelParams.WeekendSpawnChangeMulti : 1.0f))
      continue;

    SpawnParams.OverrideLevel = SpawnPoint->GetLevel();
    ANPCStore* NPCStore = GetWorld()->SpawnActor<ANPCStore>(NPCStoreClass, SpawnPoint->GetActorLocation(),
                                                            SpawnPoint->GetActorRotation(), SpawnParams);
    NPCStore->SpawnPointId = SpawnPoint->Id;

    NPCStore->InteractionComponent->InteractionType = EInteractionType::NpcStore;
  }

  TArray<ANPCStore*> FoundStores = GetAllActorsOf<ANPCStore>(GetWorld(), NPCStoreClass);
  for (ANPCStore* NPCStore : FoundStores) {
    NPCStore->InventoryComponent->ItemsArray.Empty();
    NPCStore->DialogueComponent->DialogueArray.Empty();

    NPCStore->DialogueComponent->DialogueArray = GlobalStaticDataManager->GetRandomNpcStoreDialogue();

    // Differentiate between fixed and random stores.
    auto NpcStoreType = NPCStore->NpcStoreComponent->NpcStoreType;
    if (!NPCStore->NpcStoreComponent->bFixedStoreType) {
      NpcStoreType = GetWeightedRandomItem<FNpcStoreType>(
          GlobalDataManager->NpcStoreTypesArray, [](const auto& StoreType) { return StoreType.StoreSpawnWeight; });
      NPCStore->NpcStoreComponent->NpcStoreType = NpcStoreType;
    }

    // Get random joined item + econ types using their weights.
    int32 RandomStockCount = FMath::RandRange(NpcStoreType.StockCountRange[0], NpcStoreType.StockCountRange[1]);
    TMap<TTuple<EItemType, EItemEconType>, int32> RandomTypesCountMap;
    for (int32 i = 0; i < RandomStockCount; i++) {
      EItemType RandomItemType =
          GetWeightedRandomItem<TTuple<EItemType, float>>(NpcStoreType.ItemTypeWeightMap.Array(), [](const auto& Pair) {
            return Pair.Value;
          }).Key;
      EItemEconType RandomItemEconType =
          GetWeightedRandomItem<TTuple<EItemEconType, float>>(NpcStoreType.ItemEconTypeWeightMap.Array(),
                                                              [](const auto& Pair) { return Pair.Value; })
              .Key;

      RandomTypesCountMap.FindOrAdd(TTuple<EItemType, EItemEconType>(RandomItemType, RandomItemEconType), 0)++;
    }
    check(RandomTypesCountMap.Num() > 0);

    TMap<TTuple<EItemType, EItemEconType>, TArray<FName>> PossibleItemIdsMap;
    for (const auto& Id : Market->EligibleItemIds)
      if (RandomTypesCountMap.Contains(TTuple<EItemType, EItemEconType>(Market->AllItemsMap[Id]->ItemType,
                                                                        Market->AllItemsMap[Id]->ItemEconType)))
        PossibleItemIdsMap
            .FindOrAdd(TTuple<EItemType, EItemEconType>(Market->AllItemsMap[Id]->ItemType,
                                                        Market->AllItemsMap[Id]->ItemEconType))
            .Add(Market->AllItemsMap[Id]->ItemID);

    for (const auto& Pair : RandomTypesCountMap) {
      if (!PossibleItemIdsMap.Contains(Pair.Key)) continue;

      TArray<FName> ItemIds = PossibleItemIdsMap[Pair.Key];
      for (int32 i = 0; i < Pair.Value; i++) NPCStore->InventoryComponent->AddItem(Market->GetRandomItem(ItemIds));
    }
  }
}

void AMarketLevel::InitMarketNpcs(bool bIsWeekend) {
  TArray<ANpcSpawnPoint*> SpawnPoints = GetAllActorsOf<ANpcSpawnPoint>(GetWorld(), NpcSpawnPointClass);
  check(SpawnPoints.Num() > 0);

  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.OverrideLevel = SpawnPoints[0]->GetLevel();
  SpawnParams.bNoFail = true;
  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

  for (ANpcSpawnPoint* SpawnPoint : SpawnPoints) {
    if (FMath::FRand() * 100 >= SpawnPoint->SpawnChance * (bIsWeekend ? LevelParams.WeekendSpawnChangeMulti : 1.0f))
      continue;

    if (TrySpawnUniqueNpc(SpawnPoint, SpawnParams)) continue;

    ANpc* Npc = GetWorld()->SpawnActor<ANpc>(NpcClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(),
                                             SpawnParams);
    Npc->SpawnPointId = SpawnPoint->Id;
    Npc->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;

    if (Market->TodaysEconEvents.Num() <= 0) {
      Npc->DialogueComponent->DialogueArray = GlobalStaticDataManager->GetRandomMarketNpcDialogue(
          [&](const FDialogueData& Dialogue) { return Dialogue.DialogueTags.IsEmpty(); });
      continue;
    }

    const TArray<FEconEvent>& CatastrophicEvents = Market->TodaysEconEvents.FilterByPredicate(
        [](const auto& Event) { return Event.Severity == EEconEventSeverity::Catastrophic; });
    if (CatastrophicEvents.Num() > 0 && FMath::FRand() * 100 < LevelParams.CatastrophicReactionChance) {
      Npc->DialogueComponent->DialogueArray = GlobalStaticDataManager->GetRandomMarketNpcDialogue(
          [&](const FDialogueData& Dialogue) { return Dialogue.DialogueTags.HasAny(CatastrophicEvents[0].Tags); });
      continue;
    }

    const FEconEvent& RandomEconEvent = GetWeightedRandomItem<FEconEvent>(
        Market->TodaysEconEvents, [](const auto& Event) { return Event.StartChance; });
    Npc->DialogueComponent->DialogueArray =
        GlobalStaticDataManager->GetRandomMarketNpcDialogue([&](const FDialogueData& Dialogue) {
          return Dialogue.DialogueTags.IsEmpty() || Dialogue.DialogueTags.HasAny(RandomEconEvent.Tags);
        });
  }
}

auto AMarketLevel::TrySpawnUniqueNpc(ANpcSpawnPoint* SpawnPoint,
                                     const FActorSpawnParameters& SpawnParams,
                                     bool bIsWeekend) -> bool {
  if (FMath::FRand() * 100 >=
      LevelParams.UniqueNpcBaseSpawnChance * (bIsWeekend ? LevelParams.WeekendSpawnChangeMulti : 1.0f))
    return false;

  TArray<struct FUniqueNpcData> EligibleNpcs = GlobalStaticDataManager->GetEligibleNpcs().FilterByPredicate(
      [this](const auto& Npc) { return !RecentlySpawnedUniqueNpcsMap.Contains(Npc.ID); });
  if (EligibleNpcs.Num() <= 0) return false;

  FUniqueNpcData UniqueNpcData =
      GetWeightedRandomItem<FUniqueNpcData>(EligibleNpcs, [](const auto& Npc) { return Npc.SpawnWeight; });
  RecentlySpawnedUniqueNpcsMap.Add(UniqueNpcData.ID, LevelParams.RecentNpcSpawnedKeepTime);

  ANpc* UniqueNpc = GetWorld()->SpawnActor<ANpc>(NpcClass, SpawnPoint->GetActorLocation(),
                                                 SpawnPoint->GetActorRotation(), SpawnParams);
  UniqueNpc->SpawnPointId = SpawnPoint->Id;

  UniqueNpc->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;
  UniqueNpc->DialogueComponent->DialogueArray =
      GlobalStaticDataManager->GetRandomNpcDialogue(UniqueNpcData.DialogueChainIDs);

  // Quest override.
  auto MarketQuestChains =
      QuestManager->GetEligibleQuestChains(UniqueNpcData.QuestIDs).FilterByPredicate([](const auto& Chain) {
        return Chain.CustomerAction == ECustomerAction::None;
      });
  if (MarketQuestChains.Num() <= 0) return true;

  const FQuestChainData& RandomQuestChainData =
      GetWeightedRandomItem<FQuestChainData>(MarketQuestChains, [](const auto& Chain) { return Chain.StartChance; });
  if (FMath::FRand() * 100 >= RandomQuestChainData.StartChance) return true;

  UniqueNpc->QuestComponent->ChainID = RandomQuestChainData.ID;
  UniqueNpc->QuestComponent->CustomerAction = RandomQuestChainData.CustomerAction;
  UniqueNpc->QuestComponent->QuestOutcomeType = RandomQuestChainData.QuestOutcomeType;
  UniqueNpc->InteractionComponent->InteractionType = EInteractionType::UniqueNPCQuest;
  UniqueNpc->DialogueComponent->DialogueArray =
      GlobalStaticDataManager->GetQuestDialogue(RandomQuestChainData.DialogueChainID);

  return true;
}

void AMarketLevel::InitMiniGames(bool bIsWeekend) {
  TArray<AMiniGameSpawnPoint*> SpawnPoints = GetAllActorsOf<AMiniGameSpawnPoint>(GetWorld(), MiniGameSpawnPointClass);
  check(SpawnPoints.Num() > 0);

  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.OverrideLevel = SpawnPoints[0]->GetLevel();
  SpawnParams.bNoFail = true;
  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

  TArray<EMiniGame> SpawnedMiniGameTypes;
  for (auto* SpawnPoint : SpawnPoints) {
    auto SpawnableMiniGameTypes = SpawnPoint->MiniGameTypes.FilterByPredicate(
        [this, &SpawnedMiniGameTypes](const auto& MiniGame) { return !SpawnedMiniGameTypes.Contains(MiniGame); });

    const FMiniGameInfo* SpawnableMiniGame = nullptr;
    for (const auto& MiniGame : SpawnableMiniGameTypes) {
      if (FMath::FRand() * 100 < MiniGameManager->MiniGameInfoMap[MiniGame].SpawnChance *
                                     (bIsWeekend ? LevelParams.WeekendSpawnChangeMulti : 1.0f)) {
        SpawnableMiniGame = &MiniGameManager->MiniGameInfoMap[MiniGame];
        break;
      }
    }
    if (!SpawnableMiniGame) continue;

    AMiniGame* MiniGame = GetWorld()->SpawnActor<AMiniGame>(MiniGameClass, SpawnPoint->GetActorLocation(),
                                                            SpawnPoint->GetActorRotation(), SpawnParams);
    MiniGame->SpawnPointId = SpawnPoint->Id;

    MiniGame->Mesh->SetStaticMesh(SpawnableMiniGame->Mesh);
    MiniGame->InteractionComponent->InteractionType = EInteractionType::MiniGame;
    MiniGame->DialogueComponent->DialogueArray = SpawnableMiniGame->InitDialogues;
    MiniGame->MiniGameComponent->MiniGameType = SpawnableMiniGame->MiniGame;

    SpawnedMiniGameTypes.Add(SpawnableMiniGame->MiniGame);
  }
}