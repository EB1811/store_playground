#include "Market.h"
#include "Containers/Array.h"
#include "HAL/Platform.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/NameTypes.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Interaction/InteractionComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/WorldObject/NPCStore.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Market/NpcStoreComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/WorldObject/Level/NpcSpawnPoint.h"
#include "store_playground/WorldObject/Npc.h"
#include "store_playground/Quest/QuestManager.h"
#include "store_playground/Quest/QuestComponent.h"

AMarket::AMarket() {
  PrimaryActorTick.bCanEverTick = false;

  Upgradeable.ChangeBehaviorParam = [this](const TMap<FName, float>& ParamValues) { ChangeBehaviorParam(ParamValues); };
  Upgradeable.UnlockIDs = [this](const FName DataName, const TArray<FName>& Ids) { UnlockIDs(DataName, Ids); };
}

void AMarket::BeginPlay() {
  Super::BeginPlay();

  check(AllItemsTable && NPCStoreClass);

  TArray<FItemDataRow*> ItemRows;
  AllItemsTable->GetAllRows<FItemDataRow>("", ItemRows);
  for (auto Row : ItemRows) {
    UItemBase* Item = NewObject<UItemBase>(this);

    Item->ItemID = Row->ItemID;
    Item->UniqueItemID = FGuid::NewGuid();
    Item->Quantity = 1;
    Item->ItemType = Row->ItemType;
    Item->ItemWealthType = Row->ItemWealthType;
    Item->ItemEconType = Row->ItemEconType;
    Item->TextData = Row->TextData;
    Item->AssetData = Row->AssetData;
    Item->FlavorData = Row->FlavorData;
    Item->PriceData.BoughtAt = Row->BasePrice;

    AllItemsMap.Add(Row->ItemID, Item);
    if (!Row->bIsUnlockable) EligibleItemIds.Add(Row->ItemID);
  }

  check(AllItemsMap.Num() > 0);
  check(EligibleItemIds.Num() > 0);

  AllItemsTable = nullptr;
}

void AMarket::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

auto AMarket::GetNewRandomItems(int32 Amount) const -> TArray<UItemBase*> {
  TArray<UItemBase*> NewItems;
  for (int32 i = 0; i < Amount; i++) {
    int32 RandomIndex = FMath::RandRange(0, EligibleItemIds.Num() - 1);
    NewItems.Add(AllItemsMap[EligibleItemIds[RandomIndex]]->CreateItemCopy());
  }

  return NewItems;
}

UItemBase* AMarket::GetRandomItem(const TArray<FName> ItemIds) const {
  const TArray<FName> ItemIdsEligible =
      EligibleItemIds.FilterByPredicate([ItemIds](const FName& Id) { return ItemIds.Contains(Id); });
  if (ItemIdsEligible.Num() <= 0) return nullptr;

  FName RandomId = ItemIdsEligible[FMath::RandRange(0, ItemIds.Num() - 1)];
  return AllItemsMap[RandomId]->CreateItemCopy();
}

auto AMarket::BuyItem(UNpcStoreComponent* NpcStoreC,
                      UInventoryComponent* NPCStoreInventory,
                      UInventoryComponent* PlayerInventory,
                      AStore* PlayerStore,
                      UItemBase* Item,
                      int32 Quantity) const -> bool {
  check(NpcStoreC && NPCStoreInventory && PlayerInventory && PlayerStore && Item);
  if (!NPCStoreInventory->ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->ItemID == Item->ItemID; }))
    return false;

  const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
      [Item](const FEconItem& EconItem) { return EconItem.ItemID == Item->ItemID; });
  check(EconItem);

  float StoreMarkup = NpcStoreC->NpcStoreType.StoreMarkup * BehaviorParams.StoreMarkupMulti;
  float TotalPrice = EconItem->CurrentPrice * Quantity * (1.0f + StoreMarkup);
  UE_LOG(LogTemp, Warning, TEXT("StoreMarkup: %f"), StoreMarkup);
  UE_LOG(LogTemp, Warning, TEXT("TotalPrice: %f"), TotalPrice);
  if (PlayerStore->Money < TotalPrice) return false;

  if (!TransferItem(NPCStoreInventory, PlayerInventory, Item, Quantity).bSuccess) return false;

  PlayerStore->Money -= TotalPrice;
  return true;
}

auto AMarket::SellItem(UNpcStoreComponent* NpcStoreC,
                       UInventoryComponent* NPCStoreInventory,
                       UInventoryComponent* PlayerInventory,
                       AStore* PlayerStore,
                       UItemBase* Item,
                       int32 Quantity) const -> bool {
  check(NpcStoreC && NPCStoreInventory && PlayerInventory && PlayerStore && Item);
  if (!PlayerInventory->ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->ItemID == Item->ItemID; }))
    return false;

  const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
      [Item](const FEconItem& EconItem) { return EconItem.ItemID == Item->ItemID; });
  check(EconItem);

  if (!TransferItem(PlayerInventory, NPCStoreInventory, Item, Quantity).bSuccess) return false;

  float StoreMarkup = NpcStoreC->NpcStoreType.StoreMarkup * BehaviorParams.StoreMarkupMulti;
  PlayerStore->Money += EconItem->CurrentPrice * Quantity * (1.0f - StoreMarkup);
  return true;
}

auto AMarket::ConsiderEconEvents() -> TArray<struct FEconEvent> {
  TArray<struct FEconEvent> RandomEconEvents = {};

  TArray<struct FEconEvent> EligibleEvents = GlobalDataManager->GetEligibleEconEvents(OccurredEconEvents);
  if (EligibleEvents.Num() <= 0) return {};
  for (auto& Event : EligibleEvents)
    if (RecentEconEventsMap.Contains(Event.ID)) Event.StartChance *= 0.25f;

  for (auto& Event : EligibleEvents) {
    if (FMath::FRand() * 100 >= Event.StartChance) continue;

    RandomEconEvents.Add(Event);
    TArray<struct FPriceEffect> PriceEffects = GlobalDataManager->GetPriceEffects(Event.PriceEffectIDs);
    for (const auto& PriceEffect : PriceEffects) MarketEconomy->ActivePriceEffects.Add(PriceEffect);

    OccurredEconEvents.Add(Event.ID);  // ? Don't add if repeatable?
    RecentEconEventsMap.Add(Event.ID, MarketParams.RecentEconEventsKeepTime);
  }
  // ? If no events, add a random one so there's at least one.
  // FEconEvent RandomEvent =
  //     GetWeightedRandomItem<FEconEvent>(EligibleEvents, [](const auto& Event) { return Event.StartChance; });

  return RandomEconEvents;
}

void AMarket::TickDaysTimedVars() {
  TArray<FName> NpcsToRemove;
  for (auto& Pair : RecentlySpawnedUniqueNpcsMap)
    if (Pair.Value <= 1)
      NpcsToRemove.Add(Pair.Key);
    else
      Pair.Value--;

  TArray<FName> EconEventsToRemove;
  for (auto& Pair : RecentEconEventsMap)
    if (Pair.Value <= 1)
      EconEventsToRemove.Add(Pair.Key);
    else
      Pair.Value--;

  for (const auto& NpcId : NpcsToRemove) RecentlySpawnedUniqueNpcsMap.Remove(NpcId);
  for (const auto& EventId : EconEventsToRemove) RecentEconEventsMap.Remove(EventId);
}

void AMarket::ChangeBehaviorParam(const TMap<FName, float>& ParamValues) {
  for (const auto& ParamPair : ParamValues) {
    auto StructProp = CastField<FStructProperty>(this->GetClass()->FindPropertyByName("BehaviorParams"));
    auto StructPtr = StructProp->ContainerPtrToValuePtr<void>(this);
    SetStructPropertyValue(StructProp, StructPtr, ParamPair.Key, ParamPair.Value);
  }
}

void AMarket::UnlockIDs(const FName DataName, const TArray<FName>& Ids) {
  TArray<FName> IdsFilteredDuplicates =
      Ids.FilterByPredicate([this](const FName& Id) { return !EligibleItemIds.Contains(Id); });
  for (auto Id : IdsFilteredDuplicates) EligibleItemIds.Add(Id);
}

void AMarket::SaveMarketLevelState() {
  MarketLevelState.NpcStoreSaveMap.Empty();

  TArray<ANPCStore*> FoundStores = GetAllActorsOf<ANPCStore>(GetWorld(), NPCStoreClass);
  for (ANPCStore* Store : FoundStores)
    MarketLevelState.NpcStoreSaveMap.Add(Store->NpcStoreId, {
                                                                Store->NpcStoreId,
                                                                Store->DialogueComponent->DialogueArray,
                                                                Store->InventoryComponent->ItemsArray,
                                                                Store->NpcStoreComponent->NpcStoreType,
                                                            });
}

void AMarket::LoadMarketLevelState() {
  // * Initial day's state.
  if (MarketLevelState.NpcStoreSaveMap.Num() == 0) {
    InitNPCStores();
    InitMarketNpcs();
    return;
  }

  TArray<ANPCStore*> FoundStores = GetAllActorsOf<ANPCStore>(GetWorld(), NPCStoreClass);
  for (ANPCStore* Store : FoundStores) {
    check(MarketLevelState.NpcStoreSaveMap.Contains(Store->NpcStoreId));

    FNpcStoreSaveState SaveState = MarketLevelState.NpcStoreSaveMap[Store->NpcStoreId];
    Store->NpcStoreId = SaveState.Id;
    Store->DialogueComponent->DialogueArray = SaveState.DialogueArray;
    Store->InventoryComponent->ItemsArray = SaveState.ItemsArray;
    Store->NpcStoreComponent->NpcStoreType = SaveState.NpcStoreType;
  }
}

void AMarket::ResetMarketLevelState() { MarketLevelState.NpcStoreSaveMap.Empty(); }

void AMarket::InitNPCStores() {
  check(GlobalDataManager);

  TArray<ANPCStore*> FoundStores = GetAllActorsOf<ANPCStore>(GetWorld(), NPCStoreClass);
  check(FoundStores.Num() > 0);

  for (ANPCStore* NPCStore : FoundStores) {
    NPCStore->InventoryComponent->ItemsArray.Empty();
    NPCStore->DialogueComponent->DialogueArray.Empty();

    NPCStore->DialogueComponent->DialogueArray = GlobalDataManager->GetRandomNpcStoreDialogue();

    auto NpcStoreType = GetWeightedRandomItem<FNpcStoreType>(
        GlobalDataManager->NpcStoreTypesArray, [](const auto& StoreType) { return StoreType.StoreSpawnWeight; });
    NPCStore->NpcStoreComponent->NpcStoreType = NpcStoreType;
    int32 RandomStockCount = FMath::RandRange(NpcStoreType.StockCountRange[0], NpcStoreType.StockCountRange[1]);
    // Get random joined item + econ types using their weights.
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
    for (const auto& Id : EligibleItemIds)
      if (RandomTypesCountMap.Contains(
              TTuple<EItemType, EItemEconType>(AllItemsMap[Id]->ItemType, AllItemsMap[Id]->ItemEconType)))
        PossibleItemIdsMap
            .FindOrAdd(TTuple<EItemType, EItemEconType>(AllItemsMap[Id]->ItemType, AllItemsMap[Id]->ItemEconType))
            .Add(AllItemsMap[Id]->ItemID);

    TArray<UItemBase*> StoreItems;
    for (const auto& Pair : RandomTypesCountMap) {
      if (!PossibleItemIdsMap.Contains(Pair.Key)) continue;

      TArray<FName> ItemIds = PossibleItemIdsMap[Pair.Key];
      for (int32 i = 0; i < Pair.Value; i++) NPCStore->InventoryComponent->AddItem(GetRandomItem(ItemIds));
    }
  }
}

void AMarket::InitMarketNpcs() {
  TArray<ANpcSpawnPoint*> SpawnPoints = GetAllActorsOf<ANpcSpawnPoint>(GetWorld(), NpcSpawnPointClass);
  check(SpawnPoints.Num() > 0);

  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;
  SpawnParams.OverrideLevel = SpawnPoints[0]->GetLevel();
  SpawnParams.bNoFail = true;
  SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

  for (ANpcSpawnPoint* SpawnPoint : SpawnPoints) {
    if (FMath::FRand() * 100 >= SpawnPoint->SpawnChance) continue;

    if (TrySpawnUniqueNpc(SpawnPoint, SpawnParams)) continue;

    ANpc* Npc = GetWorld()->SpawnActor<ANpc>(NpcClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(),
                                             SpawnParams);
    Npc->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;
    Npc->DialogueComponent->DialogueArray = GlobalDataManager->GetRandomCustomerDialogue();
  }
}

auto AMarket::TrySpawnUniqueNpc(ANpcSpawnPoint* SpawnPoint, const FActorSpawnParameters& SpawnParams) -> bool {
  if (FMath::FRand() * 100 >= MarketParams.UniqueNpcBaseSpawnChance) return false;

  TArray<struct FUniqueNpcData> EligibleNpcs = GlobalDataManager->GetEligibleNpcs().FilterByPredicate(
      [this](const auto& Npc) { return !RecentlySpawnedUniqueNpcsMap.Contains(Npc.ID); });
  if (EligibleNpcs.Num() <= 0) return false;

  FUniqueNpcData UniqueNpcData =
      GetWeightedRandomItem<FUniqueNpcData>(EligibleNpcs, [](const auto& Npc) { return Npc.SpawnWeight; });
  RecentlySpawnedUniqueNpcsMap.Add(UniqueNpcData.ID, MarketParams.RecentNpcSpawnedKeepTime);

  ANpc* UniqueNpc = GetWorld()->SpawnActor<ANpc>(NpcClass, SpawnPoint->GetActorLocation(),
                                                 SpawnPoint->GetActorRotation(), SpawnParams);
  UniqueNpc->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;
  UniqueNpc->DialogueComponent->DialogueArray = GlobalDataManager->GetRandomNpcDialogue(UniqueNpcData.DialogueChainIDs);

  // Quest override.
  auto MarketQuestChains =
      QuestManager->GetEligibleQuestChains(UniqueNpcData.QuestIDs).FilterByPredicate([](const auto& Chain) {
        return Chain.CustomerAction == ECustomerAction::None;
      });
  if (MarketQuestChains.Num() <= 0) return true;

  const FQuestChainData& RandomQuestChainData =
      GetWeightedRandomItem<FQuestChainData>(MarketQuestChains, [](const auto& Chain) { return Chain.StartChance; });
  if (FMath::FRand() * 100 >= RandomQuestChainData.StartChance) return true;

  UniqueNpc->QuestComponent->QuestChainData = RandomQuestChainData;
  UniqueNpc->InteractionComponent->InteractionType = EInteractionType::UniqueNPCQuest;
  UniqueNpc->DialogueComponent->DialogueArray =
      GlobalDataManager->GetQuestDialogue(RandomQuestChainData.DialogueChainID);

  return true;
}