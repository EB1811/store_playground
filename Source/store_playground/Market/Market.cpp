#include "Market.h"
#include "Containers/Array.h"
#include "HAL/Platform.h"
#include "Logging/LogVerbosity.h"
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
#include "store_playground/Framework/GlobalStaticDataManager.h"
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
  Upgradeable.UpgradeFunction = [this](const FName FunctionName, const TArray<FName>& Ids,
                                       const TMap<FName, float>& ParamValues) {
    UpgradeFunction(FunctionName, Ids, ParamValues);
  };
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
    Item->PlayerPriceData.BoughtAt = 0.0f;

    AllItemsMap.Add(Row->ItemID, Item);
    if (!Row->bIsUnlockable) EligibleItemIds.Add(Row->ItemID);
  }

  check(AllItemsMap.Num() > 0);
  check(EligibleItemIds.Num() > 0);

  AllItemsTable = nullptr;
}

void AMarket::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

auto AMarket::GetNewRandomItems(int32 Amount,
                                TArray<EItemType> ItemTypes,
                                TArray<EItemWealthType> ItemWealthTypes,
                                TArray<EItemEconType> ItemEconType,
                                std::function<bool(const FName& ItemId)> FilterFunc) const -> TArray<UItemBase*> {
  TArray<FName> ItemIdsEligible = EligibleItemIds.FilterByPredicate([&](const FName& Id) {
    const UItemBase* Item = AllItemsMap[Id];

    if (ItemTypes.Num() > 0 && !ItemTypes.Contains(Item->ItemType)) return false;
    if (ItemWealthTypes.Num() > 0 && !ItemWealthTypes.Contains(Item->ItemWealthType)) return false;
    if (ItemEconType.Num() > 0 && !ItemEconType.Contains(Item->ItemEconType)) return false;
    return true;
  });
  if (FilterFunc) ItemIdsEligible = ItemIdsEligible.FilterByPredicate([&](const FName& Id) { return FilterFunc(Id); });
  if (ItemIdsEligible.Num() <= 0) return {};

  TArray<UItemBase*> NewItems;
  for (int32 i = 0; i < Amount; i++) {
    if (ItemIdsEligible.Num() <= 0) break;

    int32 RandomIndex = FMath::RandRange(0, ItemIdsEligible.Num() - 1);
    NewItems.Add(AllItemsMap[ItemIdsEligible[RandomIndex]]->CreateItemCopy());
    ItemIdsEligible.RemoveAt(RandomIndex);
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
UItemBase* AMarket::GetRandomItemWeighted(const TArray<FName> ItemIds,
                                          std::function<float(const UItemBase* Item)> WeightFunc) const {
  const TArray<FName> ItemIdsEligible =
      EligibleItemIds.FilterByPredicate([ItemIds](const FName& Id) { return ItemIds.Contains(Id); });
  if (ItemIdsEligible.Num() <= 0) return nullptr;

  FName RandomId = GetWeightedRandomItem<FName>(
      ItemIdsEligible, [this, WeightFunc](const FName& Id) { return WeightFunc(AllItemsMap[Id]); });
  return AllItemsMap[RandomId]->CreateItemCopy();
}

UItemBase* AMarket::GetItem(const FName& ItemID) const {
  check(AllItemsMap.Contains(ItemID));
  const UItemBase* Item = AllItemsMap[ItemID];
  return Item->CreateItemCopy();
}

auto AMarket::GetNpcStoreSellPrice(const class UNpcStoreComponent* NpcStoreC, const FName& ItemID) const -> float {
  check(NpcStoreC);

  const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
      [ItemID](const FEconItem& EconItem) { return EconItem.ItemID == ItemID; });
  check(EconItem);

  float ItemMarkup = (NpcStoreC->StockItemMarkups.Contains(ItemID) ? NpcStoreC->StockItemMarkups[ItemID]
                                                                   : NpcStoreC->NpcStoreType.StoreMarkup) *
                     BehaviorParams.StoreMarkupMulti *
                     BehaviorParams.StoreMarkupItemEconTypeMulti[EconItem->ItemEconType];
  float TotalPrice = EconItem->CurrentPrice * (1.0f + ItemMarkup);
  return TotalPrice;
}
auto AMarket::GetNpcStoreBuyPrice(const class UNpcStoreComponent* NpcStoreC, const FName& ItemID) const -> float {
  check(NpcStoreC);

  const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
      [ItemID](const FEconItem& EconItem) { return EconItem.ItemID == ItemID; });
  check(EconItem);

  float ItemMarkup = (NpcStoreC->StockItemMarkups.Contains(ItemID) ? NpcStoreC->StockItemMarkups[ItemID]
                                                                   : NpcStoreC->NpcStoreType.StoreMarkup) *
                     BehaviorParams.StoreMarkupMulti *
                     BehaviorParams.StoreMarkupItemEconTypeMulti[EconItem->ItemEconType];
  float TotalPrice = EconItem->CurrentPrice * (1.0f - ItemMarkup);
  return TotalPrice;
}

auto AMarket::BuyItem(UNpcStoreComponent* NpcStoreC,
                      UInventoryComponent* NPCStoreInventory,
                      UInventoryComponent* PlayerInventory,
                      AStore* PlayerStore,
                      UItemBase* Item,
                      int32 Quantity) const -> bool {
  check(NpcStoreC && NPCStoreInventory && PlayerInventory && PlayerStore && Item);
  if (!NPCStoreInventory->ItemsArray.ContainsByPredicate(
          [Item](UItemBase* ArrayItem) { return ArrayItem->UniqueItemID == Item->UniqueItemID; }))
    return false;

  const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
      [Item](const FEconItem& EconItem) { return EconItem.ItemID == Item->ItemID; });
  check(EconItem);

  float SingleItemPrice = GetNpcStoreSellPrice(NpcStoreC, Item->ItemID);
  float TotalPrice = SingleItemPrice * Quantity;
  if (PlayerStore->Money < TotalPrice) {
    UE_LOG(LogTemp, Log, TEXT("Not enough money to buy item: %s, TotalPrice: %.0f, Money: %.0f"),
           *Item->TextData.Name.ToString(), TotalPrice, PlayerStore->Money);
    return false;
  }

  if (!CanTransferItem(PlayerInventory, Item)) return false;

  auto TransferItemRes = TransferItem(NPCStoreInventory, PlayerInventory, Item, Quantity);
  check(TransferItemRes.bSuccess);
  PlayerStore->ItemBought(TransferItemRes.ItemCopy, SingleItemPrice, Quantity);

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
          [Item](UItemBase* ArrayItem) { return ArrayItem->UniqueItemID == Item->UniqueItemID; }))
    return false;

  const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
      [Item](const FEconItem& EconItem) { return EconItem.ItemID == Item->ItemID; });
  check(EconItem);

  if (!TransferItem(PlayerInventory, NPCStoreInventory, Item, Quantity).bSuccess) return false;

  float SingleItemPrice = GetNpcStoreBuyPrice(NpcStoreC, Item->ItemID);
  PlayerStore->MoneyGained(SingleItemPrice * Quantity);
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
    if (FMath::FRand() * 100 < Event.StartChance * (EconEvents.Num() < 1 ? 2.0f : 0.5f / EconEvents.Num()) *
                                   BehaviorParams.EconEventStartChanceMulti)
      EconEvents.Add(Event);

  for (auto& Event : EconEvents) {
    TArray<struct FPriceEffect> PriceEffects = GlobalStaticDataManager->GetPriceEffects(Event.PriceEffectIDs);
    for (const auto& PriceEffect : PriceEffects) MarketEconomy->AddPriceEffect(PriceEffect);
    TArray<struct FPopEffect> PopEffects = GlobalStaticDataManager->GetPopEffects(Event.PopEffectIDs);
    for (const auto& PopEffect : PopEffects) MarketEconomy->ActivePopEffects.Add(PopEffect);

    if (!OccurredEconEvents.Contains(Event.ID)) OccurredEconEvents.Add(Event.ID);  // ? Don't add if repeatable?
    RecentEconEventsMap.Add(Event.ID, MarketParams.RecentEconEventsKeepTime);
    if (!TodaysEconEvents.ContainsByPredicate([Event](const auto& OtherEvent) { return Event.ID == OtherEvent.ID; }))
      TodaysEconEvents.Add(Event);
  }

  return EconEvents;
}

void AMarket::TickDaysTimedVars() {
  TodaysEconEvents.Empty();

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
    AddToStructPropertyValue(StructProp, StructPtr, ParamPair.Key, ParamPair.Value);
  }
}
void AMarket::UnlockIDs(const FName DataName, const TArray<FName>& Ids) {
  if (DataName != "Item") checkf(false, TEXT("UnlockIDs only supports Item IDs."));

  TArray<FName> IdsFilteredDuplicates =
      Ids.FilterByPredicate([this](const FName& Id) { return !EligibleItemIds.Contains(Id); });
  for (auto Id : IdsFilteredDuplicates) EligibleItemIds.Add(Id);
}
void AMarket::UpgradeFunction(FName FunctionName, const TArray<FName>& Ids, const TMap<FName, float>& ParamValues) {
  if (FunctionName == "ChangeItemEconTypeMulti") ChangeItemEconTypeMulti(ParamValues);
  else checkNoEntry();
}
void AMarket::ChangeItemEconTypeMulti(const TMap<FName, float>& ParamValues) {
  for (const auto& ParamPair : ParamValues) {
    EItemEconType EconType = EItemEconType::Consumer;
    for (auto T : TEnumRange<EItemEconType>()) {
      FString EnumStr;
      UEnum::GetValueAsString(T).Split(TEXT("::"), nullptr, &EnumStr);
      if (EnumStr == ParamPair.Key) {
        EconType = T;
        break;
      }
    }

    BehaviorParams.StoreMarkupItemEconTypeMulti[EconType] += ParamPair.Value;
  }
}