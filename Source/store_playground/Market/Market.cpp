#include "Market.h"
#include "HAL/Platform.h"
#include "Math/UnrealMathUtility.h"
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

AMarket::AMarket() { PrimaryActorTick.bCanEverTick = false; }

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

    AllItems.Add(Item);
  }

  check(AllItems.Num() > 0);

  AllItemsTable = nullptr;
}

void AMarket::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

TArray<UItemBase*> AMarket::GetNewRandomItems(int32 Amount) const {
  TArray<UItemBase*> NewItems;
  for (int32 i = 0; i < Amount; i++) {
    int32 RandomIndex = FMath::RandRange(0, AllItems.Num() - 1);
    NewItems.Add(AllItems[RandomIndex]->CreateItemCopy());
  }

  return NewItems;
}

UItemBase* AMarket::GetRandomItem(const TArray<FName> ItemIds) const {
  FName RandomId = ItemIds[FMath::RandRange(0, ItemIds.Num() - 1)];
  return *(AllItems.FindByPredicate([RandomId](const UItemBase* Item) { return Item->ItemID == RandomId; }));
}

bool AMarket::BuyItem(UNpcStoreComponent* NpcStoreC,
                      UInventoryComponent* NPCStoreInventory,
                      UInventoryComponent* PlayerInventory,
                      AStore* PlayerStore,
                      UItemBase* Item,
                      int32 Quantity) {
  check(NpcStoreC && NPCStoreInventory && PlayerInventory && PlayerStore && Item);
  if (!NPCStoreInventory->ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->ItemID == Item->ItemID; }))
    return false;

  const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
      [Item](const FEconItem& EconItem) { return EconItem.ItemID == Item->ItemID; });
  check(EconItem);

  float TotalPrice = EconItem->CurrentPrice * Quantity * (1.0f + (NpcStoreC->NpcStoreType.StorePriceMarkup / 100.0f));
  if (PlayerStore->Money < TotalPrice) return false;

  if (!TransferItem(NPCStoreInventory, PlayerInventory, Item, Quantity).bSuccess) return false;

  PlayerStore->Money -= TotalPrice;
  return true;
}

bool AMarket::SellItem(UNpcStoreComponent* NpcStoreC,
                       UInventoryComponent* NPCStoreInventory,
                       UInventoryComponent* PlayerInventory,
                       AStore* PlayerStore,
                       UItemBase* Item,
                       int32 Quantity) {
  check(NpcStoreC && NPCStoreInventory && PlayerInventory && PlayerStore && Item);
  if (!PlayerInventory->ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->ItemID == Item->ItemID; }))
    return false;

  const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
      [Item](const FEconItem& EconItem) { return EconItem.ItemID == Item->ItemID; });
  check(EconItem);

  if (!TransferItem(PlayerInventory, NPCStoreInventory, Item, Quantity).bSuccess) return false;

  PlayerStore->Money +=
      EconItem->CurrentPrice * Quantity * (1.0f - (NpcStoreC->NpcStoreType.StorePriceMarkup / 100.0f));
  return true;
}

FEconEvent AMarket::ConsiderEconEvent() {
  const TMap<EReqFilterOperand, std::any> GameDataMap = {{}};  // TODO: Get game data.

  TArray<struct FEconEvent> EligibleEvents = GlobalDataManager->GetEligibleEconEvents(GameDataMap, OccurredEconEvents);
  if (EligibleEvents.Num() <= 0) return {};

  for (auto& Event : EligibleEvents)
    if (RecentEconEvents.Contains(Event.EconEventID)) Event.StartChance *= 0.25f;
  FEconEvent RandomEvent =
      GetWeightedRandomItem<FEconEvent>(EligibleEvents, [](const auto& Event) { return Event.StartChance; });
  if (FMath::FRand() * 100 > RandomEvent.StartChance) return {};

  TArray<struct FPriceEffect> PriceEffects = GlobalDataManager->GetPriceEffects(RandomEvent.PriceEffectIDs);
  for (const auto& PriceEffect : PriceEffects) MarketEconomy->ActivePriceEffects.Add(PriceEffect);

  OccurredEconEvents.Add(RandomEvent.EconEventID);  // ? Don't add if repeatable?
  RecentEconEvents.Add(RandomEvent.EconEventID);

  return RandomEvent;
}

void AMarket::SaveMarketLevelState() {
  MarketLevelState.NpcStoreSaveMap.Empty();

  TArray<ANPCStore*> FoundStores = GetAllActorsOf<ANPCStore>(GetWorld(), NPCStoreClass);
  for (ANPCStore* Store : FoundStores)
    MarketLevelState.NpcStoreSaveMap.Add(Store->NpcStoreId, SaveNpcStoreSaveState(Store));
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
    LoadNpcStoreSaveState(Store, MarketLevelState.NpcStoreSaveMap[Store->NpcStoreId]);
  }
}

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
    for (const auto& Item : AllItems)
      if (RandomTypesCountMap.Contains(TTuple<EItemType, EItemEconType>(Item->ItemType, Item->ItemEconType)))
        PossibleItemIdsMap.FindOrAdd(TTuple<EItemType, EItemEconType>(Item->ItemType, Item->ItemEconType))
            .Add(Item->ItemID);

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
    UE_LOG(LogTemp, Warning, TEXT("Spawn."));
    if (FMath::FRand() * 100 >= SpawnPoint->SpawnChance) continue;

    if (TrySpawnUniqueNpc(SpawnPoint, SpawnParams)) continue;

    ANpc* Npc = GetWorld()->SpawnActor<ANpc>(NpcClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(),
                                             SpawnParams);
    Npc->InteractionComponent->InteractionType = EInteractionType::NPCDialogue;
    Npc->DialogueComponent->DialogueArray = GlobalDataManager->GetRandomCustomerDialogue();
  }
}

bool AMarket::TrySpawnUniqueNpc(ANpcSpawnPoint* SpawnPoint, const FActorSpawnParameters& SpawnParams) {
  if (FMath::FRand() * 100 >= MarketParams.UniqueNpcBaseSpawnChance) return false;
  UE_LOG(LogTemp, Warning, TEXT("Trying unique npc:"));

  const TMap<EReqFilterOperand, std::any> GameDataMap = {{}};  // Temp.
  TArray<struct FUniqueNpcData> EligibleNpcs =
      GlobalDataManager->GetEligibleNpcs(GameDataMap).FilterByPredicate([this](const auto& Npc) {
        return !RecentlySpawnedUniqueNpcIds.Contains(Npc.NpcID);
      });
  if (EligibleNpcs.Num() <= 0) return false;

  FUniqueNpcData UniqueNpcData =
      GetWeightedRandomItem<FUniqueNpcData>(EligibleNpcs, [](const auto& Npc) { return Npc.SpawnChanceWeight; });
  RecentlySpawnedUniqueNpcIds.Add(UniqueNpcData.NpcID);

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