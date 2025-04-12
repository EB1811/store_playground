#include <cmath>
#include "Containers/Array.h"
#include "Containers/Set.h"
#include "HAL/Platform.h"
#include "MarketEconomy.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/WorldObject/NPCStore.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

EPopWealthType GetHigherWealthType(EPopWealthType WealthType) {
  int32 WealthTypeIndex = static_cast<int32>(WealthType);
  if (WealthTypeIndex >= 2) return WealthType;

  return static_cast<EPopWealthType>(WealthTypeIndex + 1);
}
EPopWealthType GetLowerWealthType(EPopWealthType WealthType) {
  int32 WealthTypeIndex = static_cast<int32>(WealthType);
  if (WealthTypeIndex <= 0) return WealthType;

  return static_cast<EPopWealthType>(WealthTypeIndex - 1);
}

AMarketEconomy::AMarketEconomy() {
  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickInterval = 1;

  EconomyParams.bRunSimulation = true;
  EconomyParams.NeedsfulfilledPercent = 0.6f;
  EconomyParams.SingleUnitPriceMulti = 0.1;
  TotalPopulation = 0;
  TotalWealth = 0;
  TotaBought = 0;
}

void AMarketEconomy::BeginPlay() {
  Super::BeginPlay();

  InitializeEconomyData();
}

void AMarketEconomy::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  if (EconomyParams.bRunSimulation) PerformEconomyTick();
}

void AMarketEconomy::PerformEconomyTick() {
  // * Pop Simulation.
  // Calculate total wealth.
  TotalWealth = 0;
  for (int32 i = 0; i < CustomerPops.Num(); i++) TotalWealth += CustomerPops[i].MGen * PopEconDataArray[i].Population;
  TMap<FName, float> PopWealthMap;
  for (int32 i = 0; i < CustomerPops.Num(); i++)  // %share = diff(pop ratio, base %)
    PopWealthMap.Add(CustomerPops[i].ID, TotalWealth *
                                             ((CustomerPops[i].MSharePercent / 100.0f) +
                                              (float(PopEconDataArray[i].Population) / float(TotalPopulation))) *
                                             0.5f);

  // Calculate the amount of total goods bought, and the needs fulfilled for each pop.
  TotaBought = 0;
  TMap<TTuple<EItemEconType, EItemWealthType>, float> TotalBoughtMap;
  for (auto EconType : TEnumRange<EItemEconType>())
    for (auto WealthType : TEnumRange<EItemWealthType>()) TotalBoughtMap.Add({EconType, WealthType}, 0);

  // Data for pop promotion/demotion.
  // ? Store goods bought for each pop?
  TArray<int32> PromotionConsiderPopIndexes;
  TArray<int32> DemotionConsiderPopIndexes;

  for (int32 i = 0; i < CustomerPops.Num(); i++) {
    auto& CustomerPop = CustomerPops[i];
    auto& PopEconData = PopEconDataArray[i];

    PopEconData.Money = PopWealthMap[PopEconData.PopID];
    PopEconData.ItemNeedsFulfilled.Empty();
    for (auto WealthType : TEnumRange<EItemWealthType>()) PopEconData.ItemNeedsFulfilled.Add(WealthType, 0);

    float PopTotalBought = 0;

    TArray<EItemEconType> PopEconTypes = CustomerPop.ItemEconTypes;
    for (auto EconType : PopEconTypes) {
      float Money = PopEconData.Money / PopEconTypes.Num();
      float RemainingMoney = 0;

      TMap<EItemWealthType, float> PopBoughtMap;
      TMap<EItemWealthType, bool> NeedsFulfilledMap;
      for (auto WealthType : TEnumRange<EItemWealthType>()) {
        PopBoughtMap.Add(WealthType, 0);
        NeedsFulfilledMap.Add(WealthType, false);
      }

      // ItemSpendPercent pass.
      for (auto WealthType : TEnumRange<EItemWealthType>()) {
        float Need = CustomerPop.ItemNeeds[WealthType] * PopEconData.Population;
        float Spending = Money * (CustomerPop.ItemSpendPercent[WealthType] / 100);
        float Price = EconTypePricesMap[EconType].WealthTypePricesMap[WealthType];

        // Buy as many to reach need.
        float BuyAmount = Spending / Price;
        if (BuyAmount >= Need) {
          PopBoughtMap[WealthType] = Need;
          NeedsFulfilledMap[WealthType] = true;
          float Excess = BuyAmount - Need;
          RemainingMoney += Excess * Price;
          continue;
        }
        PopBoughtMap[WealthType] = BuyAmount;
      }

      // Remaining money sequential pass.
      for (auto WealthType : TEnumRange<EItemWealthType>()) {
        if (NeedsFulfilledMap[WealthType]) continue;
        if (RemainingMoney <= 0) break;

        float Need = CustomerPop.ItemNeeds[WealthType] * PopEconData.Population;
        float Spending = RemainingMoney;
        float Price = EconTypePricesMap[EconType].WealthTypePricesMap[WealthType];

        float AdditionalBuyAmount = Spending / Price;
        float BuyAmount = PopBoughtMap[WealthType] + AdditionalBuyAmount;
        if (BuyAmount >= Need) {
          PopBoughtMap[WealthType] = Need;
          NeedsFulfilledMap[WealthType] = true;
          float Excess = BuyAmount - Need;
          RemainingMoney += Excess * Price;
          continue;
        }
        PopBoughtMap[WealthType] = BuyAmount;
        RemainingMoney -= Spending;
      }

      // If all needs fulfilled, buy luxury items.
      if (RemainingMoney > 0) {
        float Spending = RemainingMoney;
        float Price = EconTypePricesMap[EconType].WealthTypePricesMap[EItemWealthType::Luxury];

        float BuyAmount = Spending / Price;
        PopBoughtMap[EItemWealthType::Luxury] += BuyAmount;
      }

      // Update total bought map.
      for (auto WealthType : TEnumRange<EItemWealthType>()) {
        PopTotalBought += PopBoughtMap[WealthType];
        TotaBought += PopBoughtMap[WealthType];
        PopEconData.ItemNeedsFulfilled[WealthType] +=
            PopBoughtMap[WealthType] / (CustomerPop.ItemNeeds[WealthType] * PopEconData.Population);

        TotalBoughtMap[{EconType, WealthType}] += PopBoughtMap[WealthType];
      }

      // Add a deviation in population's goods bought and check promotion/demotion.
      TArray<float> EssentialBoughtSplit =
          GetRandomSplit(std::min(PopEconData.Population, 5), PopBoughtMap[EItemWealthType::Essential]);
      float EssentialBucketNeeds = CustomerPop.ItemNeeds[EItemWealthType::Essential] * PopEconData.Population / 5;
      for (float Bought : EssentialBoughtSplit)
        if (Bought / EssentialBucketNeeds < 0.2f) DemotionConsiderPopIndexes.Add(i);

      TArray<float> LuxuryBoughtSplit =
          GetRandomSplit(std::min(PopEconData.Population, 5), PopBoughtMap[EItemWealthType::Luxury]);
      float LuxuryBucketNeeds = CustomerPop.ItemNeeds[EItemWealthType::Luxury] * PopEconData.Population / 5;
      for (float Bought : LuxuryBoughtSplit)
        if (std::max(Bought, 0.1f) / std::max(LuxuryBucketNeeds, 1.0f) > 0.8f) PromotionConsiderPopIndexes.Add(i);
    }

    PopEconData.GoodsBoughtPerCapita = PopTotalBought / PopEconData.Population;
    for (auto WealthType : TEnumRange<EItemWealthType>())
      PopEconData.ItemNeedsFulfilled[WealthType] /= PopEconTypes.Num();
  }

  // Calculate perfect prices based on amount of bought items + small random variation.
  for (auto& Item : EconItems)
    if (TotalBoughtMap[{Item.ItemEconType, Item.ItemWealthType}] > 0)
      Item.PerfectPrice = TotalBoughtMap[{Item.ItemEconType, Item.ItemWealthType}] * FMath::RandRange(0.98f, 1.02f) *
                          Item.BoughtToPriceMulti;

  // * Economic Events.
  if (ActivePriceEffects.Num() > 0) {
    for (auto& Item : EconItems) {
      float PriceAddition = 0;
      for (auto& PriceEffect : ActivePriceEffects)
        if (PriceEffect.ItemTypes.Contains(Item.ItemType) &&
            PriceEffect.ItemWealthTypes.Contains(Item.ItemWealthType) &&
            PriceEffect.ItemEconTypes.Contains(Item.ItemEconType))
          PriceAddition += Item.PerfectPrice * PriceEffect.PriceMultiPercent / 100.0f;

      Item.PerfectPrice += PriceAddition;
    }
  }

  // * Calculate current prices based on perfect prices and update WealthTypePricesMap average.
  TMap<TTuple<EItemEconType, EItemWealthType>, float> WealthTypePricesSumMap;
  TMap<TTuple<EItemEconType, EItemWealthType>, int32> ItemsCountMap;
  for (auto EconType : TEnumRange<EItemEconType>()) {
    for (auto WealthType : TEnumRange<EItemWealthType>()) {
      WealthTypePricesSumMap.Add({EconType, WealthType}, 0);
      ItemsCountMap.Add({EconType, WealthType}, 0);
    }
  }

  for (auto& Item : EconItems) {
    Item.CurrentPrice += (Item.PerfectPrice - Item.CurrentPrice) * Item.PriceJumpMulti;

    WealthTypePricesSumMap[{Item.ItemEconType, Item.ItemWealthType}] +=
        Item.CurrentPrice * EconomyParams.SingleUnitPriceMulti;
    ItemsCountMap[{Item.ItemEconType, Item.ItemWealthType}] += 1;
  }

  for (auto EconType : TEnumRange<EItemEconType>())
    for (auto WealthType : TEnumRange<EItemWealthType>())
      if (WealthTypePricesSumMap[{EconType, WealthType}] > 0 && ItemsCountMap[{EconType, WealthType}] > 0)
        EconTypePricesMap[EconType].WealthTypePricesMap[WealthType] =
            WealthTypePricesSumMap[{EconType, WealthType}] / ItemsCountMap[{EconType, WealthType}];

  // * Pop Changes.
  TArray<int32> PromotionPopIndexes;
  TArray<int32> DemotionPopIndexes;
  TArray<int32> CrossPromotionPopIndexes;
  for (int32 Index : PromotionConsiderPopIndexes)
    if (FMath::FRand() * 100 < EconomyBehaviorParams.PromotionChance &&
        GetHigherWealthType(CustomerPops[Index].WealthType) != CustomerPops[Index].WealthType)
      PromotionPopIndexes.Add(Index);
    else if (FMath::FRand() * 100 < EconomyBehaviorParams.CrossPromotionChance)
      CrossPromotionPopIndexes.Add(Index);
  for (int32 Index : DemotionConsiderPopIndexes)
    if (FMath::FRand() * 100 < EconomyBehaviorParams.DemotionChance &&
        GetLowerWealthType(CustomerPops[Index].WealthType) != CustomerPops[Index].WealthType)
      DemotionPopIndexes.Add(Index);
    else if (FMath::FRand() * 100 < EconomyBehaviorParams.CrossPromotionChance)
      CrossPromotionPopIndexes.Add(Index);

  // Promote / Demote if next wealth type buys more goods than current wealth type.
  for (int32 Index : PromotionPopIndexes) {
    UE_LOG(LogTemp, Warning, TEXT("Pop %s considering promotion"), *CustomerPops[Index].PopName.ToString());
    auto& CustomerPop = CustomerPops[Index];
    auto& PopEconData = PopEconDataArray[Index];

    EPopWealthType NewWealthType = GetHigherWealthType(CustomerPops[Index].WealthType);
    TArray<int32> NewWealthTypePopIndexes = {};
    for (int32 i = 0; i < CustomerPops.Num(); i++)
      if (CustomerPops[i].WealthType == NewWealthType && CustomerPops[i].PopType == CustomerPop.PopType &&
          PopEconDataArray[i].GoodsBoughtPerCapita > PopEconData.GoodsBoughtPerCapita)
        NewWealthTypePopIndexes.Add(i);
    if (NewWealthTypePopIndexes.Num() <= 0) break;

    auto& RandomNewPop =
        PopEconDataArray[NewWealthTypePopIndexes[FMath::RandRange(0, NewWealthTypePopIndexes.Num() - 1)]];
    PopEconData.Population -= 1;
    RandomNewPop.Population += 1;

    UE_LOG(LogTemp, Warning, TEXT("Pop %s promoted to %s"), *CustomerPops[Index].PopName.ToString(),
           *UEnum::GetDisplayValueAsText(NewWealthType).ToString());
  }
  for (int32 Index : DemotionPopIndexes) {
    UE_LOG(LogTemp, Warning, TEXT("Pop %s considering demotion"), *CustomerPops[Index].PopName.ToString());
    auto& CustomerPop = CustomerPops[Index];
    auto& PopEconData = PopEconDataArray[Index];

    EPopWealthType NewWealthType = GetLowerWealthType(CustomerPops[Index].WealthType);
    TArray<int32> NewWealthTypePopIndexes = {};
    for (int32 i = 0; i < CustomerPops.Num(); i++)
      if (CustomerPops[i].WealthType == NewWealthType && CustomerPops[i].PopType == CustomerPop.PopType &&
          PopEconDataArray[i].GoodsBoughtPerCapita > PopEconData.GoodsBoughtPerCapita)
        NewWealthTypePopIndexes.Add(i);
    if (NewWealthTypePopIndexes.Num() <= 0) break;

    auto& RandomNewPop =
        PopEconDataArray[NewWealthTypePopIndexes[FMath::RandRange(0, NewWealthTypePopIndexes.Num() - 1)]];
    PopEconData.Population -= 1;
    RandomNewPop.Population += 1;

    UE_LOG(LogTemp, Warning, TEXT("Pop %s demoted to %s"), *CustomerPops[Index].PopName.ToString(),
           *UEnum::GetDisplayValueAsText(NewWealthType).ToString());
  }
  for (int32 Index : CrossPromotionPopIndexes) {
    auto& CustomerPop = CustomerPops[Index];
    auto& PopEconData = PopEconDataArray[Index];

    TArray<int32> NewPopIndexes = {};
    for (int32 i = 0; i < CustomerPops.Num(); i++)
      if (CustomerPops[i].WealthType == CustomerPop.WealthType && CustomerPops[i].ID != CustomerPop.ID &&
          PopEconDataArray[i].GoodsBoughtPerCapita > PopEconData.GoodsBoughtPerCapita)
        NewPopIndexes.Add(i);
    if (NewPopIndexes.Num() <= 0) break;

    auto& RandomNewPop = PopEconDataArray[NewPopIndexes[FMath::RandRange(0, NewPopIndexes.Num() - 1)]];
    PopEconData.Population -= 1;
    RandomNewPop.Population += 1;

    UE_LOG(LogTemp, Warning, TEXT("Pop %s cross promoted to %s"), *CustomerPops[Index].PopName.ToString(),
           *CustomerPops[NewPopIndexes[FMath::RandRange(0, NewPopIndexes.Num() - 1)]].PopName.ToString());
  }
}

void AMarketEconomy::TickDaysActivePriceEffects() {
  TArray<FPriceEffect> PriceEffectsToRemove;

  for (auto& PriceEffect : ActivePriceEffects) {
    if (PriceEffect.DurationLeft <= 1) {
      PriceEffectsToRemove.Add(PriceEffect);
    } else {
      PriceEffect.DurationLeft -= 1;
      PriceEffect.PriceMultiPercent =
          std::max(PriceEffect.PriceMultiPercent - PriceEffect.PriceMultiPercentFalloff, 1.0f);
    }
  }

  for (auto& PriceEffect : PriceEffectsToRemove)
    ActivePriceEffects.RemoveAllSwap(
        [PriceEffect](const auto& OtherPriceEffect) { return PriceEffect.ID == OtherPriceEffect.ID; });
}

void AMarketEconomy::InitializeEconomyData() {
  int32 TotalMShare = 0;
  TMap<EPopType, TMap<EPopWealthType, FBasePopTypeEconomyData>> PopEconGenDataMap;
  TArray<FBasePopTypeToEconomyRow*> BasePopTypeToEconomyRows;
  BasePopTypeToEconomyTable->GetAllRows<FBasePopTypeToEconomyRow>("", BasePopTypeToEconomyRows);
  check(BasePopTypeToEconomyRows.Num() > 1);
  for (auto* Row : BasePopTypeToEconomyRows) {
    PopEconGenDataMap.FindOrAdd(Row->PopType);
    PopEconGenDataMap[Row->PopType].Add(Row->WealthType, {
                                                             Row->MGen,
                                                             Row->MSharePercent,
                                                             Row->ItemSpendPercent,
                                                             Row->ItemNeeds,
                                                         });
    TotalMShare += Row->MSharePercent;
  }

  // Check that all wealth types are present for each pop type.
  // for (auto PopType : TEnumRange<EPopType>()) {
  //   if (!PopEconGenDataMap.Contains(PopType))
  //     checkf(false, TEXT("PopType %s not found in PopEconGenDataMap"),
  //            *UEnum::GetDisplayValueAsText(PopType).ToString());
  //   for (auto WealthType : TEnumRange<EPopWealthType>())
  //     if (!PopEconGenDataMap[PopType].Contains(WealthType))
  //       checkf(false, TEXT("WealthType %s not found in PopEconGenDataMap for PopType %s"),
  //              *UEnum::GetDisplayValueAsText(WealthType).ToString(),
  //              *UEnum::GetDisplayValueAsText(PopType).ToString());
  // }

  UE_LOG(LogTemp, Warning, TEXT("Total MShare: %d"), TotalMShare);
  UE_LOG(LogTemp, Warning, TEXT("Total MShare: %f"), 100. / (float)TotalMShare);

  // Equalize all percentages.
  for (auto& PopType : PopEconGenDataMap) {
    for (auto& PopEconGen : PopType.Value) {
      if (TotalMShare != 100) PopEconGen.Value.MSharePercent *= 100. / (float)TotalMShare;

      int32 TotalItemSpend = 0;
      for (auto& ItemSpend : PopEconGen.Value.ItemSpendPercent) TotalItemSpend += ItemSpend.Value;
      if (TotalItemSpend != 100)
        for (auto& ItemSpend : PopEconGen.Value.ItemSpendPercent) ItemSpend.Value *= 100. / (float)TotalItemSpend;
    }
  }

  CustomerPops.Empty();
  TArray<FCustomerPopDataRow*> CustomerPopDataTableRows;
  CustomerPopDataTable->GetAllRows<FCustomerPopDataRow>("", CustomerPopDataTableRows);
  for (auto* Row : CustomerPopDataTableRows) {
    check(PopEconGenDataMap.Contains(Row->PopType));
    check(PopEconGenDataMap[Row->PopType].Contains(Row->WealthType));

    CustomerPops.Add({
        Row->ID,
        Row->PopName,
        Row->PopType,
        Row->WealthType,
        PopEconGenDataMap[Row->PopType][Row->WealthType].MGen,
        PopEconGenDataMap[Row->PopType][Row->WealthType].MSharePercent,
        Row->ItemEconTypes,
        PopEconGenDataMap[Row->PopType][Row->WealthType].ItemSpendPercent,
        PopEconGenDataMap[Row->PopType][Row->WealthType].ItemNeeds,
    });
    PopEconDataArray.Add({Row->ID, Row->InitPopulation, 0, 0});

    TotalPopulation += Row->InitPopulation;
  }

  TArray<FItemDataRow*> ItemRows;
  AllItemsTable->GetAllRows<FItemDataRow>("", ItemRows);
  for (auto Row : ItemRows) {
    EconItems.Add({
        Row->ItemID,
        Row->ItemType,
        Row->ItemEconType,
        Row->ItemWealthType,
        Row->BasePrice,
        Row->BasePrice,
        0,
        Row->PriceJumpPercent / 100,
    });
  }

  // * Data normalization mgen values.
  // Normalization = calculate the mgen given that the average needs are fulfilled.
  // Alternative would be to manually set the "right" values in the data.
  TMap<TTuple<EItemEconType, EItemWealthType>, float> AverageNeedsMap;
  TMap<TTuple<EItemEconType, EItemWealthType>, float> AverageUnitPriceMap;
  TMap<TTuple<EItemEconType, EItemWealthType>, int32> ItemCountMap;
  for (auto EconType : TEnumRange<EItemEconType>()) {
    for (auto WealthType : TEnumRange<EItemWealthType>()) {
      AverageNeedsMap.Add({EconType, WealthType}, 0);
      AverageUnitPriceMap.Add({EconType, WealthType}, 0);
      ItemCountMap.Add({EconType, WealthType}, 0);
    }
  }
  for (int32 i = 0; i < CustomerPops.Num(); i++) {
    // Preserving ratio of needs.
    for (auto EconType : CustomerPops[i].ItemEconTypes) {
      float TotalNeeds = 0;
      for (auto WealthType : TEnumRange<EItemWealthType>())
        TotalNeeds += float(CustomerPops[i].ItemNeeds[WealthType]) * float(PopEconDataArray[i].Population);
      for (auto WealthType : TEnumRange<EItemWealthType>())
        AverageNeedsMap[{EconType, WealthType}] += TotalNeeds * (CustomerPops[i].ItemSpendPercent[WealthType] / 100.0f);
    }
  }
  for (auto& Item : EconItems) {
    AverageUnitPriceMap[{Item.ItemEconType, Item.ItemWealthType}] +=
        Item.CurrentPrice * EconomyParams.SingleUnitPriceMulti;
    ItemCountMap[{Item.ItemEconType, Item.ItemWealthType}] += 1;
  }
  for (auto EconType : TEnumRange<EItemEconType>()) {
    for (auto WealthType : TEnumRange<EItemWealthType>()) {
      TArray<FCustomerPop> RelevantPops = CustomerPops.FilterByPredicate(
          [EconType, WealthType](const auto& Pop) { return Pop.ItemEconTypes.Contains(EconType); });
      AverageNeedsMap[{EconType, WealthType}] /= float(std::max(RelevantPops.Num(), 1));

      if (AverageUnitPriceMap[{EconType, WealthType}] <= 0 || ItemCountMap[{EconType, WealthType}] <= 0) {
        AverageUnitPriceMap[{EconType, WealthType}] = 1;
        continue;
      }
      AverageUnitPriceMap[{EconType, WealthType}] /= ItemCountMap[{EconType, WealthType}];
    }
  }

  float BaseTotalMoney = 0;
  for (int32 i = 0; i < CustomerPops.Num(); i++)
    BaseTotalMoney += CustomerPops[i].MGen * PopEconDataArray[i].Population;
  float ActualTotalMoney = 0;
  for (auto EconType : TEnumRange<EItemEconType>()) {
    for (auto WealthType : TEnumRange<EItemWealthType>()) {
      TArray<FCustomerPop> RelevantPops = CustomerPops.FilterByPredicate(
          [EconType, WealthType](const auto& Pop) { return Pop.ItemEconTypes.Contains(EconType); });
      ActualTotalMoney +=
          AverageUnitPriceMap[{EconType, WealthType}] *
          (AverageNeedsMap[{EconType, WealthType}] * EconomyParams.NeedsfulfilledPercent * RelevantPops.Num());
    }
  }
  float MGenMulti = (ActualTotalMoney / BaseTotalMoney);
  check(MGenMulti > 0);
  for (int32 i = 0; i < CustomerPops.Num(); i++) CustomerPops[i].MGen *= MGenMulti;

  // Calc BoughtToPriceMulti.
  for (auto& Item : EconItems) {
    TArray<FCustomerPop> RelevantPops = CustomerPops.FilterByPredicate(
        [Item](const auto& Pop) { return Pop.ItemEconTypes.Contains(Item.ItemEconType); });
    Item.BoughtToPriceMulti = Item.CurrentPrice / std::max((AverageNeedsMap[{Item.ItemEconType, Item.ItemWealthType}] *
                                                            RelevantPops.Num() * EconomyParams.NeedsfulfilledPercent),
                                                           1.0f);
  }

  // Setting WealthTypePricesMap.
  for (auto EconType : TEnumRange<EItemEconType>()) {
    EconTypePricesMap.Add(EconType, FEconTypePrices());
    for (auto WealthType : TEnumRange<EItemWealthType>())
      EconTypePricesMap[EconType].WealthTypePricesMap.Add(WealthType, 0);
  }
  for (auto EconType : TEnumRange<EItemEconType>())
    for (auto WealthType : TEnumRange<EItemWealthType>())
      EconTypePricesMap[EconType].WealthTypePricesMap[WealthType] = AverageUnitPriceMap[{EconType, WealthType}];

  BasePopTypeToEconomyTable = nullptr;
  CustomerPopDataTable = nullptr;
  AllItemsTable = nullptr;
}