
#include "Containers/Array.h"
#include "Containers/Set.h"
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

FCustomerPop RemovePop(TArray<FCustomerPop>& Pops, TArray<FPopMoneySpendData>& PopMoneySpend, FName PopID) {
  FCustomerPop* Pop = Pops.FindByPredicate([PopID](const FCustomerPop& Pop) { return Pop.PopID == PopID; });
  FPopMoneySpendData* PopSpend =
      PopMoneySpend.FindByPredicate([PopID](const FPopMoneySpendData& PopSpend) { return PopSpend.PopID == PopID; });
  check(Pop && PopSpend);

  Pop->EconData.Population -= 1;
  PopSpend->Population -= 1;

  return *Pop;
}

FCustomerPop AddPop(TArray<FCustomerPop>& Pops, TArray<FPopMoneySpendData>& PopMoneySpend, FName PopID) {
  FCustomerPop* Pop = Pops.FindByPredicate([PopID](const FCustomerPop& Pop) { return Pop.PopID == PopID; });
  FPopMoneySpendData* PopSpend =
      PopMoneySpend.FindByPredicate([PopID](const FPopMoneySpendData& PopSpend) { return PopSpend.PopID == PopID; });
  check(Pop && PopSpend);

  Pop->EconData.Population += 1;
  PopSpend->Population += 1;

  return *Pop;
}

AMarketEconomy::AMarketEconomy() {
  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickInterval = 1;

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
  PerformEconomyTick();
}

void AMarketEconomy::PerformEconomyTick() {
  // * Pop Simulation.
  // Calculate total wealth.
  TotalWealth = 0;
  for (FCustomerPop& Pop : AllCustomerPops) TotalWealth += Pop.EconData.MGen * Pop.EconData.Population;
  TMap<FName, float> PopWealthMap;
  for (FCustomerPop& Pop : AllCustomerPops)  // %share = diff(pop ratio, base %)
    PopWealthMap.Add(
        Pop.PopID,
        TotalWealth *
            ((Pop.EconData.MSharePercent / 100.0f) + (float(Pop.EconData.Population) / float(TotalPopulation))) * 0.5f);

  // Calculate the amount of total goods bought, and the needs fulfilled for each pop.
  TotaBought = 0;
  TMap<TTuple<EItemEconType, EItemWealthType>, float> TotalBoughtMap;
  for (auto EconType : TEnumRange<EItemEconType>())
    for (auto WealthType : TEnumRange<EItemWealthType>()) TotalBoughtMap.Add({EconType, WealthType}, 0);

  // Data for pop promotion/demotion.
  // ? Store goods bought for each pop?
  TArray<FPopMoneySpendData> PromotionConsiderPops;
  TArray<FPopMoneySpendData> DemotionConsiderPops;

  for (FPopMoneySpendData& PopSpend : PopMoneySpendDataArray) {
    PopSpend.Money = PopWealthMap[PopSpend.PopID];
    PopSpend.ItemNeedsFulfilled.Empty();
    for (auto WealthType : TEnumRange<EItemWealthType>()) PopSpend.ItemNeedsFulfilled.Add(WealthType, 0);

    float PopTotalBought = 0;

    TArray<EItemEconType> PopEconTypes = PopSpend.ItemEconTypes;
    for (auto EconType : PopEconTypes) {
      float Money = PopSpend.Money / PopEconTypes.Num();
      float RemainingMoney = 0;

      TMap<EItemWealthType, float> PopBoughtMap;
      TMap<EItemWealthType, bool> NeedsFulfilledMap;
      for (auto WealthType : TEnumRange<EItemWealthType>()) {
        PopBoughtMap.Add(WealthType, 0);
        NeedsFulfilledMap.Add(WealthType, false);
      }

      // ItemSpendPercent pass.
      for (auto WealthType : TEnumRange<EItemWealthType>()) {
        float Need = PopSpend.ItemNeeds[WealthType] * PopSpend.Population;
        float Spending = Money * (PopSpend.ItemSpendPercent[WealthType] / 100);
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

        float Need = PopSpend.ItemNeeds[WealthType] * PopSpend.Population;
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
        PopSpend.ItemNeedsFulfilled[WealthType] +=
            PopBoughtMap[WealthType] / (PopSpend.ItemNeeds[WealthType] * PopSpend.Population);

        TotalBoughtMap[{EconType, WealthType}] += PopBoughtMap[WealthType];
      }

      // Add a deviation in population's goods bought and check promotion/demotion.
      TArray<float> EssentialBoughtSplit =
          GetRandomSplit(std::min(PopSpend.Population, 5), PopBoughtMap[EItemWealthType::Essential]);
      float EssentialBucketNeeds = PopSpend.ItemNeeds[EItemWealthType::Essential] * PopSpend.Population / 5;
      for (float Bought : EssentialBoughtSplit)
        if (Bought / EssentialBucketNeeds < 0.2f) DemotionConsiderPops.Add(PopSpend);

      TArray<float> LuxuryBoughtSplit =
          GetRandomSplit(std::min(PopSpend.Population, 5), PopBoughtMap[EItemWealthType::Luxury]);
      float LuxuryBucketNeeds = PopSpend.ItemNeeds[EItemWealthType::Luxury] * PopSpend.Population / 5;
      for (float Bought : LuxuryBoughtSplit)
        if (std::max(Bought, 0.1f) / std::max(LuxuryBucketNeeds, 1.0f) > 0.8f) PromotionConsiderPops.Add(PopSpend);
    }

    PopSpend.GoodsBoughtPerCapita = PopTotalBought / PopSpend.Population;
    for (auto WealthType : TEnumRange<EItemWealthType>()) PopSpend.ItemNeedsFulfilled[WealthType] /= PopEconTypes.Num();
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
  TArray<FPopMoneySpendData> PromotionPops;
  TArray<FPopMoneySpendData> DemotionPops;
  TArray<FPopMoneySpendData> CrossPromotionPops;
  for (auto PopSpend : PromotionConsiderPops)
    if (FMath::FRand() < EconomyParams.BasePromotionChance &&
        GetHigherWealthType(PopSpend.WealthType) != PopSpend.WealthType)
      PromotionPops.Add(PopSpend);
    else if (FMath::FRand() < EconomyParams.BaseCrossPromotionChance)
      CrossPromotionPops.Add(PopSpend);
  for (auto PopSpend : DemotionConsiderPops)
    if (FMath::FRand() < EconomyParams.BaseDemotionChance &&
        GetLowerWealthType(PopSpend.WealthType) != PopSpend.WealthType)
      DemotionPops.Add(PopSpend);
    else if (FMath::FRand() < EconomyParams.BaseCrossPromotionChance)
      CrossPromotionPops.Add(PopSpend);

  // Promote / Demote if next wealth type buys more goods than current wealth type.
  for (FPopMoneySpendData Pop : PromotionPops) {
    UE_LOG(LogTemp, Warning, TEXT("Pop %s considering promotion"), *Pop.PopID.ToString());

    EPopWealthType NewWealthType = GetHigherWealthType(Pop.WealthType);
    TArray<FPopMoneySpendData> NewWealthTypePops =
        PopMoneySpendDataArray.FilterByPredicate([Pop, NewWealthType](const auto& OtherPop) {
          return OtherPop.WealthType == NewWealthType && OtherPop.PopType == Pop.PopType &&
                 OtherPop.GoodsBoughtPerCapita > Pop.GoodsBoughtPerCapita;
        });
    if (NewWealthTypePops.Num() <= 0) break;

    FPopMoneySpendData RandomNewPop = NewWealthTypePops[FMath::RandRange(0, NewWealthTypePops.Num() - 1)];

    RemovePop(AllCustomerPops, PopMoneySpendDataArray, Pop.PopID);
    AddPop(AllCustomerPops, PopMoneySpendDataArray, RandomNewPop.PopID);

    UE_LOG(LogTemp, Warning, TEXT("Pop %s promoted to %s"), *Pop.PopID.ToString(),
           *UEnum::GetDisplayValueAsText(NewWealthType).ToString());
  }
  for (FPopMoneySpendData Pop : DemotionPops) {
    EPopWealthType NewWealthType = GetLowerWealthType(Pop.WealthType);
    TArray<FPopMoneySpendData> NewWealthTypePops =
        PopMoneySpendDataArray.FilterByPredicate([Pop, NewWealthType](const auto& OtherPop) {
          return OtherPop.WealthType == NewWealthType && OtherPop.PopType == Pop.PopType &&
                 OtherPop.GoodsBoughtPerCapita > Pop.GoodsBoughtPerCapita;
        });
    if (NewWealthTypePops.Num() <= 0) break;

    FPopMoneySpendData RandomNewPop = NewWealthTypePops[FMath::RandRange(0, NewWealthTypePops.Num() - 1)];

    RemovePop(AllCustomerPops, PopMoneySpendDataArray, Pop.PopID);
    AddPop(AllCustomerPops, PopMoneySpendDataArray, RandomNewPop.PopID);

    UE_LOG(LogTemp, Warning, TEXT("Pop %s demoted to %s"), *Pop.PopID.ToString(),
           *UEnum::GetDisplayValueAsText(NewWealthType).ToString());
  }
  for (FPopMoneySpendData Pop : CrossPromotionPops) {
    TArray<FPopMoneySpendData> NewPops = PopMoneySpendDataArray.FilterByPredicate([Pop](const auto& OtherPop) {
      return OtherPop.WealthType == Pop.WealthType && OtherPop.PopID != Pop.PopID &&
             OtherPop.GoodsBoughtPerCapita > Pop.GoodsBoughtPerCapita;
    });
    if (NewPops.Num() <= 0) break;

    FPopMoneySpendData RandomNewPop = NewPops[FMath::RandRange(0, NewPops.Num() - 1)];

    RemovePop(AllCustomerPops, PopMoneySpendDataArray, Pop.PopID);
    AddPop(AllCustomerPops, PopMoneySpendDataArray, RandomNewPop.PopID);

    UE_LOG(LogTemp, Warning, TEXT("Pop %s changed to %s"), *Pop.PopID.ToString(), *RandomNewPop.PopID.ToString());
  }
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

  AllCustomerPops.Empty();
  TArray<FCustomerPopDataRow*> CustomerPopDataTableRows;
  CustomerPopDataTable->GetAllRows<FCustomerPopDataRow>("", CustomerPopDataTableRows);
  for (auto* Row : CustomerPopDataTableRows) {
    check(PopEconGenDataMap.Contains(Row->PopType));
    check(PopEconGenDataMap[Row->PopType].Contains(Row->WealthType));

    FPopEconData PopEconData = {
        Row->PopID,
        Row->InitPopulation,
        PopEconGenDataMap[Row->PopType][Row->WealthType].MGen,
        PopEconGenDataMap[Row->PopType][Row->WealthType].MSharePercent,
        Row->ItemEconTypes,
        PopEconGenDataMap[Row->PopType][Row->WealthType].ItemSpendPercent,
        PopEconGenDataMap[Row->PopType][Row->WealthType].ItemNeeds,
    };

    AllCustomerPops.Add({Row->PopID,
                         Row->PopName,
                         Row->PopType,
                         Row->WealthType,
                         Row->ItemEconTypes,
                         {
                             Row->PopID,
                             Row->InitPopulation,
                             PopEconData.MGen,
                             PopEconData.MSharePercent,
                             PopEconData.ItemEconTypes,
                             PopEconData.ItemSpendPercent,
                             PopEconData.ItemNeeds,
                         }});
    PopMoneySpendDataArray.Add({Row->PopID, 0.0f, PopEconData.Population, Row->PopType, Row->WealthType,
                                PopEconData.ItemEconTypes, PopEconData.ItemSpendPercent, PopEconData.ItemNeeds});

    TotalPopulation += PopEconData.Population;
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
  for (auto& Pop : AllCustomerPops) {
    // Preserving ratio of needs.
    for (auto EconType : Pop.ItemEconTypes) {
      float TotalNeeds = 0;
      for (auto WealthType : TEnumRange<EItemWealthType>())
        TotalNeeds += float(Pop.EconData.ItemNeeds[WealthType]) * float(Pop.EconData.Population);
      for (auto WealthType : TEnumRange<EItemWealthType>())
        AverageNeedsMap[{EconType, WealthType}] += TotalNeeds * (Pop.EconData.ItemSpendPercent[WealthType] / 100.0f);
    }
  }
  for (auto& Item : EconItems) {
    AverageUnitPriceMap[{Item.ItemEconType, Item.ItemWealthType}] +=
        Item.CurrentPrice * EconomyParams.SingleUnitPriceMulti;
    ItemCountMap[{Item.ItemEconType, Item.ItemWealthType}] += 1;
  }
  for (auto EconType : TEnumRange<EItemEconType>()) {
    for (auto WealthType : TEnumRange<EItemWealthType>()) {
      TArray<FPopMoneySpendData> RelevantPops = PopMoneySpendDataArray.FilterByPredicate(
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
  for (auto& Pop : AllCustomerPops) BaseTotalMoney += Pop.EconData.MGen * Pop.EconData.Population;
  float ActualTotalMoney = 0;
  for (auto EconType : TEnumRange<EItemEconType>()) {
    for (auto WealthType : TEnumRange<EItemWealthType>()) {
      TArray<FPopMoneySpendData> RelevantPops = PopMoneySpendDataArray.FilterByPredicate(
          [EconType, WealthType](const auto& Pop) { return Pop.ItemEconTypes.Contains(EconType); });
      ActualTotalMoney +=
          AverageUnitPriceMap[{EconType, WealthType}] *
          (AverageNeedsMap[{EconType, WealthType}] * EconomyParams.NeedsfulfilledPercent * RelevantPops.Num());
    }
  }
  float MGenMulti = (ActualTotalMoney / BaseTotalMoney);
  check(MGenMulti > 0);
  for (auto& Pop : AllCustomerPops) Pop.EconData.MGen *= MGenMulti;

  // Calc BoughtToPriceMulti.
  for (auto& Item : EconItems) {
    TArray<FPopMoneySpendData> RelevantPops = PopMoneySpendDataArray.FilterByPredicate(
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