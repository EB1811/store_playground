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
#include "store_playground/Quest/QuestManager.h"

AMarket::AMarket() {
  PrimaryActorTick.bCanEverTick = false;

  Upgradeable.ChangeBehaviorParam = [this](const TMap<FName, float>& ParamValues) { ChangeBehaviorParam(ParamValues); };
  Upgradeable.UnlockIDs = [this](const FName DataName, const TArray<FName>& Ids) { UnlockIDs(DataName, Ids); };
}

void AMarket::BeginPlay() {
  Super::BeginPlay();

  check(AllItemsTable);

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

// TODO: pass in wanted item types control item rarity.
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

void AMarket::AddGuaranteedEconEvents(const TArray<FName>& EconEventIDs) {
  GuaranteedEconEventIDs.Append(EconEventIDs);
}

auto AMarket::ConsiderEconEvents() -> TArray<struct FEconEvent> {
  TArray<struct FEconEvent> EconEvents = {};

  auto GuaranteedEconEvents = GlobalDataManager->GetEconEventsByIds(GuaranteedEconEventIDs);
  EconEvents.Append(GuaranteedEconEvents);
  GuaranteedEconEventIDs.Empty();

  TArray<struct FEconEvent> EligibleEvents = GlobalDataManager->GetEligibleEconEvents(OccurredEconEvents);
  if (EligibleEvents.Num() <= 0) return {};
  for (auto& Event : EligibleEvents)
    if (RecentEconEventsMap.Contains(Event.ID)) Event.StartChance *= 0.25f;
  for (auto& Event : EligibleEvents)
    if (FMath::FRand() * 100 < Event.StartChance) EconEvents.Add(Event);

  for (auto& Event : EconEvents) {
    TArray<struct FPriceEffect> PriceEffects = GlobalDataManager->GetPriceEffects(Event.PriceEffectIDs);
    for (const auto& PriceEffect : PriceEffects) MarketEconomy->ActivePriceEffects.Add(PriceEffect);

    if (!OccurredEconEvents.Contains(Event.ID)) OccurredEconEvents.Add(Event.ID);  // ? Don't add if repeatable?
    RecentEconEventsMap.Add(Event.ID, MarketParams.RecentEconEventsKeepTime);
  }
  // ? If no events, add a random one so there's at least one.
  // FEconEvent RandomEvent =
  //     GetWeightedRandomItem<FEconEvent>(EligibleEvents, [](const auto& Event) { return Event.StartChance; });

  return EconEvents;
}

void AMarket::TickDaysTimedVars() {
  TArray<FName> EconEventsToRemove;
  for (auto& Pair : RecentEconEventsMap)
    if (Pair.Value <= 1) EconEventsToRemove.Add(Pair.Key);
    else Pair.Value--;

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
  if (DataName != "Item") checkf(false, TEXT("UnlockIDs only supports Item IDs."));

  TArray<FName> IdsFilteredDuplicates =
      Ids.FilterByPredicate([this](const FName& Id) { return !EligibleItemIds.Contains(Id); });
  for (auto Id : IdsFilteredDuplicates) EligibleItemIds.Add(Id);
}