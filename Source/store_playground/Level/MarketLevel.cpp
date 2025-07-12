#include "MarketLevel.h"
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
#include "store_playground/WorldObject/NPCStore.h"
#include "store_playground/WorldObject/MobileNPCStore.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
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

  TArray<AMobileNPCStore*> FoundMobileStores = GetAllActorsOf<AMobileNPCStore>(GetWorld(), MobileNPCStoreClass);
  for (AMobileNPCStore* MobileStore : FoundMobileStores) {
    FActorSavaState ActorSaveState = SaveManager->SaveActor(MobileStore, MobileStore->SpawnPointId);

    auto InteractionCSaveState = SaveManager->SaveComponent(MobileStore->InteractionComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("InteractionComponent", InteractionCSaveState.Id);
    auto DialogueCSaveState = SaveManager->SaveComponent(MobileStore->DialogueComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("DialogueComponent", DialogueCSaveState.Id);
    auto NpcStoreCSaveState = SaveManager->SaveComponent(MobileStore->NpcStoreComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("NpcStoreComponent", NpcStoreCSaveState.Id);

    auto MeshSaveState = SaveManager->SaveMesh(MobileStore->Mesh, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("Mesh", MeshSaveState.Id);
    auto SpriteSaveState = SaveManager->SaveMesh(MobileStore->Sprite, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("Sprite", SpriteSaveState.Id);

    auto [InventorySaveState, FObjectSaveStates] =
        SaveManager->SaveInventoryCSaveState(MobileStore->InventoryComponent);
    ActorSaveState.ActorComponentsMap.Add("InventoryComponent", InventorySaveState.Id);

    LevelState.ActorSaveMap.Add(ActorSaveState.Id, ActorSaveState);
    LevelState.ComponentSaveMap.Add(InteractionCSaveState.Id, InteractionCSaveState);
    LevelState.ComponentSaveMap.Add(DialogueCSaveState.Id, DialogueCSaveState);
    LevelState.ComponentSaveMap.Add(NpcStoreCSaveState.Id, NpcStoreCSaveState);
    LevelState.ComponentSaveMap.Add(MeshSaveState.Id, MeshSaveState);
    LevelState.ComponentSaveMap.Add(SpriteSaveState.Id, SpriteSaveState);
    LevelState.ComponentSaveMap.Add(InventorySaveState.Id, InventorySaveState);
    for (FObjectSaveState& ObjectSaveState : FObjectSaveStates) LevelState.ObjectSaveStates.Add(ObjectSaveState);
  }

  TArray<ANpc*> Npcs = GetAllActorsOf<ANpc>(GetWorld(), NpcClass);
  for (auto* Npc : Npcs) {
    FActorSavaState ActorSaveState = SaveManager->SaveActor(Npc, Npc->SpawnPointId);  // Using SpawnPoint's id.

    auto InteractionCSaveState = SaveManager->SaveComponent(Npc->InteractionComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("InteractionComponent", InteractionCSaveState.Id);
    auto DialogueCSaveState = SaveManager->SaveComponent(Npc->DialogueComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("DialogueComponent", DialogueCSaveState.Id);
    auto QuestCSaveState = SaveManager->SaveComponent(Npc->QuestComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("QuestComponent", QuestCSaveState.Id);
    auto SimpleSpriteAnimCSaveState = SaveManager->SaveComponent(Npc->SimpleSpriteAnimComponent, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("SimpleSpriteAnimComponent", SimpleSpriteAnimCSaveState.Id);

    LevelState.ActorSaveMap.Add(ActorSaveState.Id, ActorSaveState);
    LevelState.ComponentSaveMap.Add(InteractionCSaveState.Id, InteractionCSaveState);
    LevelState.ComponentSaveMap.Add(DialogueCSaveState.Id, DialogueCSaveState);
    LevelState.ComponentSaveMap.Add(QuestCSaveState.Id, QuestCSaveState);
    LevelState.ComponentSaveMap.Add(SimpleSpriteAnimCSaveState.Id, SimpleSpriteAnimCSaveState);
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

    auto MeshSaveState = SaveManager->SaveMesh(MiniGame->Mesh, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("Mesh", MeshSaveState.Id);
    auto SpriteSaveState = SaveManager->SaveMesh(MiniGame->Sprite, FGuid::NewGuid());
    ActorSaveState.ActorComponentsMap.Add("Sprite", SpriteSaveState.Id);

    LevelState.ActorSaveMap.Add(ActorSaveState.Id, ActorSaveState);
    LevelState.ComponentSaveMap.Add(InteractionCSaveState.Id, InteractionCSaveState);
    LevelState.ComponentSaveMap.Add(DialogueCSaveState.Id, DialogueCSaveState);
    LevelState.ComponentSaveMap.Add(MiniGameCSaveState.Id, MiniGameCSaveState);
    LevelState.ComponentSaveMap.Add(MeshSaveState.Id, MeshSaveState);
    LevelState.ComponentSaveMap.Add(SpriteSaveState.Id, SpriteSaveState);
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

  FActorSpawnParameters NpcStoreSpawnParams;
  NpcStoreSpawnParams.Owner = this;
  NpcStoreSpawnParams.bNoFail = true;
  NpcStoreSpawnParams.SpawnCollisionHandlingOverride =
      ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

  TArray<ANpcStoreSpawnPoint*> MobileStoreSpawnPoints =
      GetAllActorsOf<ANpcStoreSpawnPoint>(GetWorld(), NpcStoreSpawnPointClass);
  check(MobileStoreSpawnPoints.Num() > 0);
  for (ANpcStoreSpawnPoint* SpawnPoint : MobileStoreSpawnPoints) {
    if (!LevelState.ActorSaveMap.Contains(SpawnPoint->Id)) continue;
    FActorSavaState NpcStoreSaveState = LevelState.ActorSaveMap[SpawnPoint->Id];
    auto InteractionCSaveState =
        LevelState.ComponentSaveMap[NpcStoreSaveState.ActorComponentsMap["InteractionComponent"]];
    auto DialogueCSaveState = LevelState.ComponentSaveMap[NpcStoreSaveState.ActorComponentsMap["DialogueComponent"]];
    auto NpcStoreCSaveState = LevelState.ComponentSaveMap[NpcStoreSaveState.ActorComponentsMap["NpcStoreComponent"]];

    auto NpcStoreMeshSaveState = LevelState.ComponentSaveMap[NpcStoreSaveState.ActorComponentsMap["Mesh"]];
    auto NpcStoreSpriteSaveState = LevelState.ComponentSaveMap[NpcStoreSaveState.ActorComponentsMap["Sprite"]];

    FComponentSaveState InventorySaveState =
        LevelState.ComponentSaveMap[NpcStoreSaveState.ActorComponentsMap["InventoryComponent"]];
    TArray<FObjectSaveState> ComponentObjectSaveStates =
        LevelState.ObjectSaveStates.FilterByPredicate([InventorySaveState](const FObjectSaveState& ObjectSaveState) {
          return InventorySaveState.ComponentObjects.Contains(ObjectSaveState.Id);
        });

    NpcStoreSpawnParams.OverrideLevel = SpawnPoint->GetLevel();
    AMobileNPCStore* SpawnedNpcStore = GetWorld()->SpawnActor<AMobileNPCStore>(
        MobileNPCStoreClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(), NpcStoreSpawnParams);
    SaveManager->LoadActor(SpawnedNpcStore, NpcStoreSaveState);
    SaveManager->LoadComponent(SpawnedNpcStore->InteractionComponent, InteractionCSaveState);
    SaveManager->LoadComponent(SpawnedNpcStore->DialogueComponent, DialogueCSaveState);
    SaveManager->LoadComponent(SpawnedNpcStore->NpcStoreComponent, NpcStoreCSaveState);
    SaveManager->LoadMesh(SpawnedNpcStore->Mesh, NpcStoreMeshSaveState);
    SaveManager->LoadMesh(SpawnedNpcStore->Sprite, NpcStoreSpriteSaveState);
    SaveManager->LoadInventoryCSaveState(SpawnedNpcStore->InventoryComponent, InventorySaveState,
                                         ComponentObjectSaveStates);
  }

  FActorSpawnParameters NpcSpawnParams;
  NpcSpawnParams.Owner = this;
  NpcSpawnParams.bNoFail = true;
  NpcSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

  TArray<ANpcSpawnPoint*> NpcSpawnPoints = GetAllActorsOf<ANpcSpawnPoint>(GetWorld(), NpcSpawnPointClass);
  check(NpcSpawnPoints.Num() > 0);
  for (ANpcSpawnPoint* SpawnPoint : NpcSpawnPoints) {
    if (!LevelState.ActorSaveMap.Contains(SpawnPoint->Id)) continue;
    FActorSavaState NpcSaveState = LevelState.ActorSaveMap[SpawnPoint->Id];
    auto InteractionCSaveState = LevelState.ComponentSaveMap[NpcSaveState.ActorComponentsMap["InteractionComponent"]];
    auto DialogueCSaveState = LevelState.ComponentSaveMap[NpcSaveState.ActorComponentsMap["DialogueComponent"]];
    auto QuestCSaveState = LevelState.ComponentSaveMap[NpcSaveState.ActorComponentsMap["QuestComponent"]];
    auto SimpleSpriteAnimCSaveState =
        LevelState.ComponentSaveMap[NpcSaveState.ActorComponentsMap["SimpleSpriteAnimComponent"]];

    NpcSpawnParams.OverrideLevel = SpawnPoint->GetLevel();
    ANpc* SpawnedNpc = GetWorld()->SpawnActor<ANpc>(NpcClass, SpawnPoint->GetActorLocation(),
                                                    SpawnPoint->GetActorRotation(), NpcSpawnParams);
    SaveManager->LoadActor(SpawnedNpc, NpcSaveState);
    SaveManager->LoadComponent(SpawnedNpc->InteractionComponent, InteractionCSaveState);
    SaveManager->LoadComponent(SpawnedNpc->DialogueComponent, DialogueCSaveState);
    SaveManager->LoadComponent(SpawnedNpc->QuestComponent, QuestCSaveState);
    SaveManager->LoadComponent(SpawnedNpc->SimpleSpriteAnimComponent, SimpleSpriteAnimCSaveState);

    SpawnedNpc->SimpleSpriteAnimComponent->Idle(SpawnedNpc->SimpleSpriteAnimComponent->CurrentDirection);
  }

  FActorSpawnParameters MinigameSpawnParams;
  MinigameSpawnParams.Owner = this;
  MinigameSpawnParams.bNoFail = true;
  MinigameSpawnParams.SpawnCollisionHandlingOverride =
      ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

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
    auto MiniGameMeshSaveState = LevelState.ComponentSaveMap[MiniGameSaveState.ActorComponentsMap["Mesh"]];
    auto MiniGameSpriteSaveState = LevelState.ComponentSaveMap[MiniGameSaveState.ActorComponentsMap["Sprite"]];

    MinigameSpawnParams.OverrideLevel = SpawnPoint->GetLevel();
    AMiniGame* SpawnedMiniGame = GetWorld()->SpawnActor<AMiniGame>(MiniGameClass, SpawnPoint->GetActorLocation(),
                                                                   SpawnPoint->GetActorRotation(), MinigameSpawnParams);
    SaveManager->LoadActor(SpawnedMiniGame, MiniGameSaveState);
    SaveManager->LoadComponent(SpawnedMiniGame->InteractionComponent, InteractionCSaveState);
    SaveManager->LoadComponent(SpawnedMiniGame->DialogueComponent, DialogueCSaveState);
    SaveManager->LoadComponent(SpawnedMiniGame->MiniGameComponent, MiniGameCSaveState);
    SaveManager->LoadMesh(SpawnedMiniGame->Mesh, MiniGameMeshSaveState);
    SaveManager->LoadMesh(SpawnedMiniGame->Sprite, MiniGameSpriteSaveState);
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

  TArray<TTuple<UNpcStoreComponent*, UInventoryComponent*>> NpcStoreComponents;

  TArray<ANpcStoreSpawnPoint*> SpawnPoints = GetAllActorsOf<ANpcStoreSpawnPoint>(GetWorld(), NpcStoreSpawnPointClass);
  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.bNoFail = true;
  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
  for (auto* SpawnPoint : SpawnPoints) {
    if (FMath::FRand() * 100 >= SpawnPoint->SpawnChance * (bIsWeekend ? LevelParams.WeekendSpawnChangeMulti : 1.0f))
      continue;

    SpawnParams.OverrideLevel = SpawnPoint->GetLevel();
    AMobileNPCStore* MobileNPCStore = GetWorld()->SpawnActor<AMobileNPCStore>(
        MobileNPCStoreClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(), SpawnParams);
    MobileNPCStore->SpawnPointId = SpawnPoint->Id;

    FNpcStoreType NpcStoreType = GetWeightedRandomItem<FNpcStoreType>(
        GlobalDataManager->NpcStoreTypesArray, [](const auto& StoreType) { return StoreType.StoreSpawnWeight; });
    MobileNPCStore->NpcStoreComponent->NpcStoreType = NpcStoreType;

    MobileNPCStore->Mesh->SetStaticMesh(NpcStoreType.AssetData.Mesh);
    MobileNPCStore->Sprite->SetFlipbook(NpcStoreType.AssetData.Sprites[ESimpleSpriteDirection::Down]);

    MobileNPCStore->InteractionComponent->InteractionType = EInteractionType::NpcStore;
    MobileNPCStore->DialogueComponent->SpeakerName = NpcStoreType.DisplayName;
    MobileNPCStore->DialogueComponent->DialogueArray = GlobalStaticDataManager->GetRandomNpcStoreDialogue();

    NpcStoreComponents.Add({MobileNPCStore->NpcStoreComponent, MobileNPCStore->InventoryComponent});
  }

  TArray<ANPCStore*> FoundStores = GetAllActorsOf<ANPCStore>(GetWorld(), NPCStoreClass);
  for (ANPCStore* NPCStore : FoundStores) {
    NPCStore->InventoryComponent->ItemsArray.Empty();
    NPCStore->DialogueComponent->DialogueArray.Empty();

    NPCStore->InteractionComponent->InteractionType = EInteractionType::NpcStore;
    NPCStore->DialogueComponent->SpeakerName = NPCStore->NpcStoreComponent->NpcStoreType.DisplayName;
    NPCStore->DialogueComponent->DialogueArray = GlobalStaticDataManager->GetRandomNpcStoreDialogue();

    NpcStoreComponents.Add({NPCStore->NpcStoreComponent, NPCStore->InventoryComponent});
  }

  for (TTuple<UNpcStoreComponent*, UInventoryComponent*> StoreComponents : NpcStoreComponents) {
    UNpcStoreComponent* NpcStoreC = StoreComponents.Key;
    UInventoryComponent* InventoryC = StoreComponents.Value;
    check(NpcStoreC->NpcStoreType.StockCountRange.Num() > 0);

    // TArray<EItemType> ItemTypes = {};
    // NpcStoreC->NpcStoreType.ItemTypeWeightMap.GetKeys(ItemTypes);
    // TArray<EItemEconType> ItemEconType = {};
    // NpcStoreC->NpcStoreType.ItemEconTypeWeightMap.GetKeys(ItemEconType);
    // TArray<UItemBase*> RandomItems = Market->GetNewRandomItems(RandomStockCount, ItemTypes, {}, ItemEconType);

    // Get random joined item + econ types using their weights.
    int32 RandomStockCount =
        FMath::RandRange(NpcStoreC->NpcStoreType.StockCountRange[0], NpcStoreC->NpcStoreType.StockCountRange[1]);
    TMap<TTuple<EItemType, EItemEconType>, int32> RandomTypesCountMap = {};
    for (int32 i = 0; i < RandomStockCount; i++) {
      EItemType RandomItemType =
          GetWeightedRandomItem<TTuple<EItemType, float>>(NpcStoreC->NpcStoreType.ItemTypeWeightMap.Array(),
                                                          [](const auto& Pair) { return Pair.Value; })
              .Key;
      EItemEconType RandomItemEconType =
          GetWeightedRandomItem<TTuple<EItemEconType, float>>(NpcStoreC->NpcStoreType.ItemEconTypeWeightMap.Array(),
                                                              [](const auto& Pair) { return Pair.Value; })
              .Key;

      RandomTypesCountMap.FindOrAdd(TTuple<EItemType, EItemEconType>(RandomItemType, RandomItemEconType), 0)++;
    }
    check(RandomTypesCountMap.Num() > 0);

    TMap<TTuple<EItemType, EItemEconType>, TArray<FName>> PossibleItemIdsMap = {};
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
      for (int32 i = 0; i < Pair.Value; i++) InventoryC->AddItem(Market->GetRandomItem(ItemIds));

      UE_LOG(LogTemp, Log, TEXT("NpcStore %s has %d items for type %s and econ type %s"),
             *NpcStoreC->NpcStoreType.DisplayName.ToString(), ItemIds.Num(), *UEnum::GetValueAsString(Pair.Key.Key),
             *UEnum::GetValueAsString(Pair.Key.Value));
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

  TArray<TTuple<FGenericCustomerData, float>> WeightedCustomers;
  for (const auto& GenericCustomer : GlobalStaticDataManager->GenericCustomersArray) {
    const auto PopData = *MarketEconomy->PopEconDataArray.FindByPredicate(
        [GenericCustomer](const auto& Pop) { return Pop.PopID == GenericCustomer.LinkedPopID; });
    WeightedCustomers.Add({GenericCustomer, PopData.Population});  // * Only looking at population.
  }

  for (ANpcSpawnPoint* SpawnPoint : SpawnPoints) {
    if (FMath::FRand() * 100 >= SpawnPoint->SpawnChance * (bIsWeekend ? LevelParams.WeekendSpawnChangeMulti : 1.0f))
      continue;

    if (TrySpawnUniqueNpc(SpawnPoint, SpawnParams)) continue;

    ANpc* Npc = GetWorld()->SpawnActor<ANpc>(NpcClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(),
                                             SpawnParams);
    Npc->SpawnPointId = SpawnPoint->Id;
    Npc->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;

    const FGenericCustomerData RandomNpcData =
        GetWeightedRandomItem<TTuple<FGenericCustomerData, float>>(WeightedCustomers, [](const auto& Item) {
          return Item.Value;
        }).Key;

    Npc->SimpleSpriteAnimComponent->IdleSprites = RandomNpcData.AssetData.IdleSprites;
    Npc->SimpleSpriteAnimComponent->WalkSprites = RandomNpcData.AssetData.WalkSprites;
    Npc->SimpleSpriteAnimComponent->Idle(static_cast<ESimpleSpriteDirection>(FMath::RandRange(0, 3)));

    Npc->DialogueComponent->SpeakerName = RandomNpcData.CustomerName;

    if (FMath::FRand() < 0.1) Npc->DynamicTalkingWidgetComponent->SetVisibility(true, true);

    if (Market->TodaysEconEvents.Num() <= 0) {
      Npc->DialogueComponent->DialogueArray =
          GlobalStaticDataManager->GetRandomMarketNpcDialogue([&](const FDialogueData& Dialogue) {
            FGameplayTagContainer EventTags = Dialogue.DialogueTags.Filter(StringTagsToContainer({"Event"}));
            if (!EventTags.IsEmpty()) return false;

            FGameplayTagContainer NpcDialogueTags =
                Dialogue.DialogueTags.Filter(StringTagsToContainer({"Npc.Dialogue"}));
            if (!NpcDialogueTags.IsEmpty() && !NpcDialogueTags.HasAny(RandomNpcData.Tags)) return false;

            return true;
          });
      continue;
    }

    const TArray<FEconEvent>& CatastrophicEvents = Market->TodaysEconEvents.FilterByPredicate(
        [](const auto& Event) { return Event.Severity == EEconEventSeverity::Catastrophic; });
    if (CatastrophicEvents.Num() > 0 && FMath::FRand() * 100 < LevelParams.CatastrophicReactionChance) {
      Npc->DialogueComponent->DialogueArray =
          GlobalStaticDataManager->GetRandomMarketNpcDialogue([&](const FDialogueData& Dialogue) {
            FGameplayTagContainer EventTags = Dialogue.DialogueTags.Filter(StringTagsToContainer({"Event"}));
            if (!EventTags.HasAny(CatastrophicEvents[0].Tags)) return false;

            FGameplayTagContainer NpcDialogueTags =
                Dialogue.DialogueTags.Filter(StringTagsToContainer({"Npc.Dialogue"}));
            if (!NpcDialogueTags.IsEmpty() && !NpcDialogueTags.HasAny(RandomNpcData.Tags)) return false;

            return true;
          });
      continue;
    }
    const FEconEvent& RandomEconEvent = GetWeightedRandomItem<FEconEvent>(
        Market->TodaysEconEvents, [](const auto& Event) { return Event.StartChance; });
    // ? Use rolling or random dialogue component type?
    Npc->DialogueComponent->DialogueArray =
        GlobalStaticDataManager->GetRandomMarketNpcDialogue([&](const FDialogueData& Dialogue) {
          FGameplayTagContainer EventTags = Dialogue.DialogueTags.Filter(StringTagsToContainer({"Event"}));
          if (!EventTags.IsEmpty() && !EventTags.HasAny(RandomEconEvent.Tags)) return false;

          FGameplayTagContainer NpcDialogueTags = Dialogue.DialogueTags.Filter(StringTagsToContainer({"Npc.Dialogue"}));
          if (!NpcDialogueTags.IsEmpty() && !NpcDialogueTags.HasAny(RandomNpcData.Tags)) return false;

          return true;
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

  UniqueNpc->SimpleSpriteAnimComponent->IdleSprites = UniqueNpcData.AssetData.IdleSprites;
  UniqueNpc->SimpleSpriteAnimComponent->WalkSprites = UniqueNpcData.AssetData.WalkSprites;

  UniqueNpc->SimpleSpriteAnimComponent->Idle(static_cast<ESimpleSpriteDirection>(FMath::RandRange(0, 3)));

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
  UniqueNpc->DialogueComponent->SpeakerName = UniqueNpcData.NpcName;
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
  UE_LOG(LogTemp, Warning, TEXT("SPAWNING"));
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

    MiniGame->Mesh->SetStaticMesh(SpawnableMiniGame->AssetData.Mesh);
    UE_LOG(LogTemp, Warning, TEXT("Mesh: %s"), *SpawnableMiniGame->AssetData.Mesh->GetName());
    MiniGame->Sprite->SetFlipbook(SpawnableMiniGame->AssetData.Sprites[ESimpleSpriteDirection::Down]);

    MiniGame->InteractionComponent->InteractionType = EInteractionType::MiniGame;
    MiniGame->DialogueComponent->SpeakerName = SpawnableMiniGame->DisplayName;
    MiniGame->DialogueComponent->DialogueArray = SpawnableMiniGame->InitDialogues;
    MiniGame->MiniGameComponent->MiniGameType = SpawnableMiniGame->MiniGame;

    SpawnedMiniGameTypes.Add(SpawnableMiniGame->MiniGame);
  }
}