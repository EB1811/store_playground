#include <cmath>
#include "Containers/Array.h"
#include "Containers/Set.h"
#include "HAL/Platform.h"
#include "Logging/LogVerbosity.h"
#include "MarketEconomy.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "store_playground/WorldObject/NPCStore.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

inline TArray<float> GetRandomSplit(int32 Buckets, float Value) {
  float Sum = 0;
  TArray<float> RandomMoneyBuckets;
  for (int i = 0; i < Buckets; i++) {
    float RandomRange = FMath::RandRange(0.3f, 0.7f);  // * Avoiding too high of a range.
    RandomMoneyBuckets.Add(RandomRange);
    Sum += RandomRange;
  }

  float scale = Value / Sum;
  for (int i = 0; i < Buckets; i++) RandomMoneyBuckets[i] = std::round(RandomMoneyBuckets[i] * scale);

  return RandomMoneyBuckets;
}

inline EPopWealthType GetHigherWealthType(EPopWealthType WealthType) {
  int32 WealthTypeIndex = static_cast<int32>(WealthType);
  if (WealthTypeIndex >= 2) return WealthType;

  return static_cast<EPopWealthType>(WealthTypeIndex + 1);
}
inline EPopWealthType GetLowerWealthType(EPopWealthType WealthType) {
  int32 WealthTypeIndex = static_cast<int32>(WealthType);
  if (WealthTypeIndex <= 0) return WealthType;

  return static_cast<EPopWealthType>(WealthTypeIndex - 1);
}

inline void MovePopPopulation(FPopEconData& From, FPopEconData& To, float PopulationPercent, int32 MaxPopChange) {
  int32 PopulationChange =
      FMath::Min(FMath::RoundToInt32(FMath::Max(From.Population * PopulationPercent, 1.0f)), MaxPopChange);
  UE_LOG(LogTemp, Warning, TEXT("PopulationChange num %d"), PopulationChange);

  From.Population -= PopulationChange;
  To.Population += PopulationChange;
}

AMarketEconomy::AMarketEconomy() {
  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickInterval = 10.0f;

  EconomyParams.bRunSimulation = true;
  EconomyParams.NeedsfulfilledPercent = 0.6f;
  EconomyParams.SingleUnitPriceMulti = 0.1;
  EconomyParams.EssentialNeedsPercent = 20.0f;
  EconomyParams.LuxaryNeedsPercent = 80.0f;
  TotalPopulation = 0;
  TotalWealth = 0;
  TotaBought = 0;

  SetupUpgradeable();
}

void AMarketEconomy::BeginPlay() {
  Super::BeginPlay();

  InitializeEconomyData();
}

void AMarketEconomy::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  if (EconomyParams.bRunSimulation) PerformEconomyTick();
}

auto AMarketEconomy::GetMarketPrice(const FName ItemId) const -> float {
  const FEconItem* EconItem =
      EconItems.FindByPredicate([ItemId](const FEconItem& Item) { return Item.ItemID == ItemId; });
  check(EconItem);

  return EconItem->CurrentPrice;
}

void AMarketEconomy::PerformEconomyTick() {
  // Note: Save file using a different customer data table will not satisfy this check.
  for (int32 i = 0; i < CustomerPops.Num(); i++) {
    const FCustomerPop Pop = CustomerPops[i];
    const FPopEconData PopEconData = PopEconDataArray[i];

    check(Pop.ID == PopEconData.PopID);
  }

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
          GetRandomSplit(FMath::Min(PopEconData.Population, 5), PopBoughtMap[EItemWealthType::Essential]);
      float EssentialBucketNeeds = CustomerPop.ItemNeeds[EItemWealthType::Essential] * PopEconData.Population / 5;
      float EssentialNeedsPercent = FMath::Max(EconomyParams.EssentialNeedsPercent / 100.0f, 0.01f);
      for (float Bought : EssentialBoughtSplit) {
        // UE_LOG(LogTemp, Warning,
        //        TEXT("pop id: %s, EssentialBought: %f, EssentialBucketNeeds: %f, EssentialNeedsPercent: %f"),
        //        *PopEconData.PopID.ToString(), Bought, EssentialBucketNeeds, EssentialNeedsPercent);
        if (Bought / EssentialBucketNeeds < EssentialNeedsPercent) DemotionConsiderPopIndexes.Add(i);
      }

      TArray<float> LuxuryBoughtSplit =
          GetRandomSplit(FMath::Min(PopEconData.Population, 5), PopBoughtMap[EItemWealthType::Luxury]);
      float LuxuryBucketNeeds = CustomerPop.ItemNeeds[EItemWealthType::Luxury] * PopEconData.Population / 5;
      float LuxaryNeedsPercent = FMath::Max(EconomyParams.LuxaryNeedsPercent / 100.0f, 0.01f);
      for (float Bought : LuxuryBoughtSplit)
        if (FMath::Max(Bought, 0.1f) / FMath::Max(LuxuryBucketNeeds, 1.0f) > LuxaryNeedsPercent)
          PromotionConsiderPopIndexes.Add(i);
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
          PriceAddition += Item.PerfectPrice * PriceEffect.CurrentPriceMultiPercent / 100.0f;

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
  TArray<int32> PromotionPopIndexes = {};
  TArray<int32> DemotionPopIndexes = {};
  TArray<int32> CrossPromotionPopIndexes = {};
  for (int32 Index : PromotionConsiderPopIndexes)
    if (FMath::FRand() * 100 < BehaviorParams.PromotionChance &&
        GetHigherWealthType(CustomerPops[Index].WealthType) != CustomerPops[Index].WealthType)
      PromotionPopIndexes.Add(Index);
    else if (FMath::FRand() * 100 < BehaviorParams.CrossPromotionChanceMap[CustomerPops[Index].WealthType] *
                                        BehaviorParams.CrossPromotionChanceMulti)
      CrossPromotionPopIndexes.Add(Index);
  for (int32 Index : DemotionConsiderPopIndexes)
    if (FMath::FRand() * 100 < BehaviorParams.DemotionChance &&
        GetLowerWealthType(CustomerPops[Index].WealthType) != CustomerPops[Index].WealthType)
      DemotionPopIndexes.Add(Index);
    else if (FMath::FRand() * 100 < BehaviorParams.CrossPromotionChanceMap[CustomerPops[Index].WealthType] *
                                        BehaviorParams.CrossPromotionChanceMulti)
      CrossPromotionPopIndexes.Add(Index);

  // Promote / Demote if next wealth type buys more goods than current wealth type.
  for (int32 Index : PromotionPopIndexes) {
    // UE_LOG(LogTemp, Warning, TEXT("Pop %s considering promotion"), *CustomerPops[Index].PopName.ToString());
    auto& CustomerPop = CustomerPops[Index];
    auto& PopEconData = PopEconDataArray[Index];

    EPopWealthType NewWealthType = GetHigherWealthType(CustomerPops[Index].WealthType);
    TArray<TTuple<int32, float>> NewWealthTypePopIndexes = {};
    for (int32 i = 0; i < CustomerPops.Num(); i++)
      if (CustomerPops[i].WealthType == NewWealthType && CustomerPops[i].PopType == CustomerPop.PopType &&
          PopEconDataArray[i].GoodsBoughtPerCapita > PopEconData.GoodsBoughtPerCapita)
        NewWealthTypePopIndexes.Add(
            {i, PopEconDataArray[i].GoodsBoughtPerCapita * GetPopWeightingMulti(CustomerPops[i])});
    if (NewWealthTypePopIndexes.Num() <= 0) continue;

    int32 RandomNewPopIndex =
        GetWeightedRandomItem<TTuple<int32, float>>(NewWealthTypePopIndexes, [](const TTuple<int, float>& Index) {
          return Index.Get<0>();
        }).Get<0>();
    auto& RandomNewPop = PopEconDataArray[RandomNewPopIndex];

    MovePopPopulation(PopEconData, RandomNewPop, 0.01f, BehaviorParams.MaxPopChangeAtOnce);

    UE_LOG(LogTemp, Log, TEXT("Pop %s promoted to %s"), *CustomerPops[Index].PopName.ToString(),
           *UEnum::GetDisplayValueAsText(NewWealthType).ToString());
  }
  for (int32 Index : DemotionPopIndexes) {
    // UE_LOG(LogTemp, Warning, TEXT("Pop %s considering demotion"), *CustomerPops[Index].PopName.ToString());
    auto& CustomerPop = CustomerPops[Index];
    auto& PopEconData = PopEconDataArray[Index];

    EPopWealthType NewWealthType = GetLowerWealthType(CustomerPops[Index].WealthType);
    TArray<TTuple<int32, float>> NewWealthTypePopIndexes = {};
    for (int32 i = 0; i < CustomerPops.Num(); i++)
      if (CustomerPops[i].WealthType == NewWealthType && CustomerPops[i].PopType == CustomerPop.PopType &&
          PopEconDataArray[i].GoodsBoughtPerCapita > PopEconData.GoodsBoughtPerCapita)
        NewWealthTypePopIndexes.Add(
            {i, PopEconDataArray[i].GoodsBoughtPerCapita * GetPopWeightingMulti(CustomerPops[i])});
    if (NewWealthTypePopIndexes.Num() <= 0) continue;

    int32 RandomNewPopIndex =
        GetWeightedRandomItem<TTuple<int32, float>>(NewWealthTypePopIndexes, [](const TTuple<int, float>& Index) {
          return Index.Get<1>();
        }).Get<0>();
    auto& RandomNewPop = PopEconDataArray[RandomNewPopIndex];

    MovePopPopulation(PopEconData, RandomNewPop, 0.01f, BehaviorParams.MaxPopChangeAtOnce);

    UE_LOG(LogTemp, Log, TEXT("Pop %s demoted to %s"), *CustomerPops[Index].PopName.ToString(),
           *UEnum::GetDisplayValueAsText(NewWealthType).ToString());
  }
  for (int32 Index : CrossPromotionPopIndexes) {
    auto& CustomerPop = CustomerPops[Index];
    auto& PopEconData = PopEconDataArray[Index];

    TArray<TTuple<int32, float>> NewPopIndexes = {};
    for (int32 i = 0; i < CustomerPops.Num(); i++)
      if (CustomerPops[i].WealthType == CustomerPop.WealthType && CustomerPops[i].ID != CustomerPop.ID &&
          PopEconDataArray[i].GoodsBoughtPerCapita > PopEconData.GoodsBoughtPerCapita)
        NewPopIndexes.Add({i, PopEconDataArray[i].GoodsBoughtPerCapita * GetPopWeightingMulti(CustomerPops[i])});
    if (NewPopIndexes.Num() <= 0) continue;

    int32 RandomNewPopIndex =
        GetWeightedRandomItem<TTuple<int32, float>>(NewPopIndexes, [](const TTuple<int, float>& Index) {
          return Index.Get<1>();
        }).Get<0>();
    auto& RandomNewPop = PopEconDataArray[RandomNewPopIndex];
    MovePopPopulation(PopEconData, RandomNewPop, 0.01f, BehaviorParams.MaxPopChangeAtOnce);

    UE_LOG(LogTemp, Log, TEXT("Pop %s cross promoted to %s"), *CustomerPops[Index].PopName.ToString(),
           *CustomerPops[RandomNewPopIndex].PopName.ToString());
  }

  // * Update statistics.
  for (auto& Item : EconItems) StatisticsGen->ItemPriceChange(Item.ItemID, Item.CurrentPrice);
  for (auto& PopEconData : PopEconDataArray)
    StatisticsGen->PopChange(PopEconData.PopID, PopEconData.Population, PopEconData.GoodsBoughtPerCapita);
  StatisticsGen->CalcNetWorth();
}

auto AMarketEconomy::GetPopWeightingMulti(const FCustomerPop& Pop) const -> float {
  float TotalMulti = 1.0f;
  for (auto& PopEffect : ActivePopEffects)
    if (PopEffect.PopTypes.Contains(Pop.PopType) && PopEffect.PopWealthTypes.Contains(Pop.WealthType))
      TotalMulti *= PopEffect.PopChangeMulti;

  return TotalMulti;
}
auto AMarketEconomy::GetPopWeightingMulti(const FPopEconData& PopEconData) const -> float {
  FCustomerPop Pop =
      *CustomerPops.FindByPredicate([PopEconData](const auto& Pop) { return Pop.ID == PopEconData.PopID; });

  float TotalMulti = 1.0f;
  for (auto& PopEffect : ActivePopEffects)
    if (PopEffect.PopTypes.Contains(Pop.PopType) && PopEffect.PopWealthTypes.Contains(Pop.WealthType))
      TotalMulti *= PopEffect.PopChangeMulti;

  return TotalMulti;
}

void AMarketEconomy::AddPriceEffect(const FPriceEffect& PriceEffect) {
  FPriceEffect NewPriceEffect = PriceEffect;
  NewPriceEffect.CurrentPriceMultiPercent =
      PriceEffect.PriceMultiPercentBuildup > 0.0f
          ? PriceEffect.PriceMultiPercent * (PriceEffect.PriceMultiPercentBuildup / 100.0f)
          : PriceEffect.PriceMultiPercent;

  ActivePriceEffects.Add(NewPriceEffect);
}

void AMarketEconomy::TickDaysActivePriceEffects() {
  TArray<FPriceEffect> PriceEffectsToRemove;
  TArray<FPopEffect> PopEffectsToRemove;

  for (auto& PriceEffect : ActivePriceEffects) {
    if (PriceEffect.DurationType == EEffectDurationType::Permanent) continue;

    if (PriceEffect.DurationLeft < 1) {
      PriceEffectsToRemove.Add(PriceEffect);
    } else if (FMath::Abs(PriceEffect.CurrentPriceMultiPercent) <
               FMath::Abs(PriceEffect.PriceMultiPercent) - KINDA_SMALL_NUMBER) {
      PriceEffect.CurrentPriceMultiPercent +=
          PriceEffect.PriceMultiPercent * (PriceEffect.PriceMultiPercentBuildup / 100.0f);
      PriceEffect.CurrentPriceMultiPercent =
          PriceEffect.PriceMultiPercent > 0.0f
              ? FMath::Min(PriceEffect.CurrentPriceMultiPercent, PriceEffect.PriceMultiPercent)
              : FMath::Max(PriceEffect.CurrentPriceMultiPercent, PriceEffect.PriceMultiPercent);
    } else {
      PriceEffect.DurationLeft -= 1;
      PriceEffect.PriceMultiPercent =
          PriceEffect.PriceMultiPercent > 0.0f
              ? FMath::Max(PriceEffect.PriceMultiPercent - PriceEffect.PriceMultiPercentFalloff, 0.0f)
              : FMath::Min(PriceEffect.PriceMultiPercent + PriceEffect.PriceMultiPercentFalloff, -0.0f);
      PriceEffect.CurrentPriceMultiPercent = PriceEffect.PriceMultiPercent;
    }
  }
  for (auto& PopEffect : ActivePopEffects) {
    if (PopEffect.DurationType == EEffectDurationType::Permanent) continue;

    if (PopEffect.DurationLeft <= 1) {
      PopEffectsToRemove.Add(PopEffect);
    } else {
      PopEffect.DurationLeft -= 1;
      PopEffect.PopChangeMulti = FMath::Max(PopEffect.PopChangeMulti - PopEffect.PopChangeMultiFalloff, 1.0f);
    }
  }

  for (auto& PriceEffect : PriceEffectsToRemove)
    ActivePriceEffects.RemoveAllSwap(
        [PriceEffect](const auto& OtherPriceEffect) { return PriceEffect.ID == OtherPriceEffect.ID; });
  for (auto& PopEffect : PopEffectsToRemove)
    ActivePopEffects.RemoveAllSwap(
        [PopEffect](const auto& OtherPopEffect) { return PopEffect.ID == OtherPopEffect.ID; });
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
        Row->AssetData,
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
      AverageNeedsMap[{EconType, WealthType}] /= float(FMath::Max(RelevantPops.Num(), 1));

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
    Item.BoughtToPriceMulti =
        Item.CurrentPrice / FMath::Max((AverageNeedsMap[{Item.ItemEconType, Item.ItemWealthType}] * RelevantPops.Num() *
                                        EconomyParams.NeedsfulfilledPercent),
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

void AMarketEconomy::SetupUpgradeable() {
  Upgradeable.ChangeBehaviorParam = [this](const TMap<FName, float>& ParamValues) {
    for (const auto& ParamPair : ParamValues) {
      auto StructProp = CastField<FStructProperty>(this->GetClass()->FindPropertyByName("BehaviorParams"));
      auto StructPtr = StructProp->ContainerPtrToValuePtr<void>(this);
      AddToStructPropertyValue(StructProp, StructPtr, ParamPair.Key, ParamPair.Value);
    }
  };
}