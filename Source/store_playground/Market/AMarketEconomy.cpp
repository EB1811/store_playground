
#include "MarketEconomy.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/WorldObject/NPCStore.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AMarketEconomy::AMarketEconomy() {
  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickInterval = 1;

  EconomyParams.SingleUnitPriceMulti = 0.1;
  TotalPopulation = 0;
  TotalWealth = 0;
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
  // Calculate total wealth.
  TotalWealth = 0;
  for (FPopEconGenData& PopGen : PopEconGenDataArray) TotalWealth += PopGen.MGen * PopGen.Population;
  TMap<FGuid, float> PopWealthMap;
  for (FPopEconGenData& PopGen : PopEconGenDataArray)  // %share = diff(pop ratio, base %)
    PopWealthMap.Add(PopGen.PopID,
                     TotalWealth * (((PopGen.MSharePercent / 100) + (PopGen.Population / TotalPopulation)) * 0.5));

  // Calculate the amount of total goods bought, and the needs fulfilled for each pop.
  TMap<EItemWealthType, float> TotalBoughtMap;
  for (auto WealthType : TEnumRange<EItemWealthType>()) TotalBoughtMap.Add(WealthType, 0);
  // ? Record bought items for each pop?
  TArray<FGuid> NeedsFulfilledPops;
  TArray<FGuid> NeedsNotClosePops;

  for (FPopMoneySpendData& PopSpend : PopMoneySpendDataArray) {
    PopSpend.Money = PopWealthMap[PopSpend.PopID];

    int32 Buckets = std::min(PopSpend.Population, 4);
    TArray<float> MoneySplit = GetRandomMoneySplit(Buckets, PopSpend.Money);

    TMap<EItemWealthType, float> PopBoughtMap;
    TMap<EItemWealthType, int> BucketFulfilledAmountMap;
    TMap<EItemWealthType, int> BucketNotCloseAmountMap;
    for (auto WealthType : TEnumRange<EItemWealthType>()) {
      PopBoughtMap.Add(WealthType, 0);
      BucketFulfilledAmountMap.Add(WealthType, 0);
      BucketNotCloseAmountMap.Add(WealthType, 0);
    }
    float RemainingMoney = 0;

    for (float Money : MoneySplit) {
      TMap<EItemWealthType, float> BucketBoughtMap;
      TMap<EItemWealthType, bool> NeedsFulfilledMap;
      for (auto WealthType : TEnumRange<EItemWealthType>()) {
        BucketBoughtMap.Add(WealthType, 0);
        NeedsFulfilledMap.Add(WealthType, false);
      }
      float BucketRemainingMoney = 0;

      // ItemSpendPercent pass.
      for (auto WealthType : TEnumRange<EItemWealthType>()) {
        float Need = PopSpend.ItemNeeds[WealthType] * (PopSpend.Population / Buckets);
        float Spending = Money * (PopSpend.ItemSpendPercent[WealthType] / 100);
        float Price = WealthTypePricesMap[WealthType];

        // Buy as many to reach need.
        float BuyAmount = Spending / Price;
        if (BuyAmount >= Need) {
          BucketBoughtMap[WealthType] = Need;
          NeedsFulfilledMap[WealthType] = true;
          float Excess = BuyAmount - Need;
          BucketRemainingMoney += Excess * Price;
          continue;
        }
        BucketBoughtMap[WealthType] = BuyAmount;
      }

      // Remaining money sequential pass.
      for (auto WealthType : TEnumRange<EItemWealthType>()) {
        if (NeedsFulfilledMap[WealthType]) continue;

        int32 Need = PopSpend.ItemNeeds[WealthType] * (PopSpend.Population / Buckets);
        float Spending = BucketRemainingMoney;
        float Price = WealthTypePricesMap[WealthType];

        float AdditionalBuyAmount = Spending / Price;
        float BuyAmount = BucketBoughtMap[WealthType] + AdditionalBuyAmount;
        if (BuyAmount >= Need) {
          BucketBoughtMap[WealthType] = Need;
          NeedsFulfilledMap[WealthType] = true;
          float Excess = BuyAmount - Need;
          BucketRemainingMoney = Excess * Price;
          continue;
        }
        BucketBoughtMap[WealthType] = BuyAmount;
        BucketRemainingMoney -= Spending;

        float BoughtNeedsPercent = BucketBoughtMap[WealthType] / Need;
        if (BoughtNeedsPercent < 0.25f) BucketNotCloseAmountMap[WealthType] += 1;
      }

      for (auto WealthType : TEnumRange<EItemWealthType>()) {
        PopBoughtMap[WealthType] += BucketBoughtMap[WealthType];
        if (NeedsFulfilledMap[WealthType]) BucketFulfilledAmountMap[WealthType] += 1;
      }
      RemainingMoney += BucketRemainingMoney;
    }

    // Evenly buy items.
    if (RemainingMoney > 0) {
      float RemainingSpending = RemainingMoney / PopBoughtMap.Num();  // Proxy for EItemWealthType num.
      for (auto WealthType : TEnumRange<EItemWealthType>()) {
        float Spending = RemainingSpending;
        float Price = WealthTypePricesMap[WealthType];

        float BuyAmount = Spending / Price;
        PopBoughtMap[WealthType] += BuyAmount;
      }
    }

    // Update total bought map.
    for (auto WealthType : TEnumRange<EItemWealthType>()) {
      TotalBoughtMap[WealthType] += PopBoughtMap[WealthType];
      if (BucketFulfilledAmountMap[WealthType] >= 1) NeedsFulfilledPops.Add(PopSpend.PopID);
      if (BucketNotCloseAmountMap[WealthType] >= 1) NeedsNotClosePops.Add(PopSpend.PopID);
    }
  }

  // Calculate perfect prices based on amount of bought items + small random variation.
  for (auto& Item : EconItems)
    if (TotalBoughtMap[Item.ItemWealthType] > 0) {
      Item.PerfectPrice =
          TotalBoughtMap[Item.ItemWealthType] * FMath::RandRange(0.95f, 1.05f) * Item.BoughtToPriceMulti;
    }

  // Additional price adjustments.
  // TODO: Implement.

  // Calculate current prices based on perfect prices and update WealthTypePricesMap average.
  TMap<EItemWealthType, float> WealthTypePricesSumMap;
  TMap<EItemWealthType, int32> ItemsCountMap;
  for (auto WealthType : TEnumRange<EItemWealthType>()) {
    WealthTypePricesSumMap.Add(WealthType, 0);
    ItemsCountMap.Add(WealthType, 0);
  }

  for (auto& Item : EconItems) {
    Item.CurrentPrice += (Item.PerfectPrice - Item.CurrentPrice) * Item.PriceJumpMulti;

    WealthTypePricesSumMap[Item.ItemWealthType] += Item.CurrentPrice * EconomyParams.SingleUnitPriceMulti;
    ItemsCountMap[Item.ItemWealthType] += 1;
  }

  for (auto WealthType : TEnumRange<EItemWealthType>())
    if (WealthTypePricesSumMap[WealthType] > 0 && ItemsCountMap[WealthType] > 0)
      WealthTypePricesMap[WealthType] = WealthTypePricesSumMap[WealthType] / ItemsCountMap[WealthType];

  UE_LOG(LogTemp, Warning, TEXT("NeedsFulfilledPops %d"), NeedsFulfilledPops.Num());
  UE_LOG(LogTemp, Warning, TEXT("NeedsNotClosePops %d"), NeedsNotClosePops.Num());

  // Consider pop changes.
  // TODO: Implement.
}

void AMarketEconomy::InitializeEconomyData() {
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
  }

  check(PopEconGenDataMap[EPopType::Common].Contains(EPopWealthType::Middle));

  AllCustomerPops.Empty();
  TArray<FCustomerPopDataRow*> CustomerPopDataTableRows;
  CustomerPopDataTable->GetAllRows<FCustomerPopDataRow>("", CustomerPopDataTableRows);
  for (auto* Row : CustomerPopDataTableRows) {
    FGuid PopID = FGuid::NewGuid();
    FPopEconData PopEconData = {
        PopID,
        Row->InitPopulation,
        PopEconGenDataMap[Row->PopType][Row->WealthType].MGen,
        PopEconGenDataMap[Row->PopType][Row->WealthType].MSharePercent,
        PopEconGenDataMap[Row->PopType][Row->WealthType].ItemSpendPercent,
        PopEconGenDataMap[Row->PopType][Row->WealthType].ItemNeeds,
    };

    AllCustomerPops.Add({PopID,
                         Row->PopName,
                         Row->PopType,
                         Row->WealthType,
                         Row->ItemEconTypes,
                         {
                             PopID,
                             Row->InitPopulation,
                             PopEconData.MGen,
                             PopEconData.MSharePercent,
                             PopEconData.ItemSpendPercent,
                             PopEconData.ItemNeeds,
                         }});
    PopEconGenDataArray.Add({PopID, PopEconData.Population, PopEconData.MGen, PopEconData.MSharePercent});
    PopMoneySpendDataArray.Add({PopID, 0, PopEconData.Population, PopEconData.ItemSpendPercent, PopEconData.ItemNeeds});

    TotalPopulation += PopEconData.Population;
  }

  TArray<FItemDataRow*> ItemRows;
  AllItemsTable->GetAllRows<FItemDataRow>("", ItemRows);
  for (auto Row : ItemRows) {
    EconItems.Add({
        Row->ItemID,
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
  TMap<EItemWealthType, float> AverageNeedsMap;
  TMap<EItemWealthType, float> AveragePriceMap;
  TMap<EItemWealthType, int32> ItemCountMap;
  for (auto WealthType : TEnumRange<EItemWealthType>()) {
    AverageNeedsMap.Add(WealthType, 0);
    AveragePriceMap.Add(WealthType, 0);
    ItemCountMap.Add(WealthType, 0);
  }
  for (auto& Pop : PopMoneySpendDataArray)
    for (auto WealthType : TEnumRange<EItemWealthType>()) AverageNeedsMap[WealthType] += Pop.ItemNeeds[WealthType];
  for (auto& Item : EconItems) {
    AveragePriceMap[Item.ItemWealthType] += Item.CurrentPrice;
    ItemCountMap[Item.ItemWealthType] += 1;
  }
  for (auto WealthType : TEnumRange<EItemWealthType>()) {
    AverageNeedsMap[WealthType] /= PopMoneySpendDataArray.Num();

    if (AveragePriceMap[WealthType] <= 0 || ItemCountMap[WealthType] <= 0) {
      AveragePriceMap[WealthType] = 1;
      continue;
    }
    AveragePriceMap[WealthType] /= ItemCountMap[WealthType];
  }

  float NeedsfulfilledPercent = 0.5;  // Artificially set for 60% of the needs fulfilled at the start of the game.
  float BaseTotalMoney = 0;
  float ActualTotalMoney = 0;
  for (auto& Pop : AllCustomerPops) {
    BaseTotalMoney += Pop.EconData.MGen;
    for (auto WealthType : TEnumRange<EItemWealthType>())
      ActualTotalMoney += (AverageNeedsMap[WealthType] * NeedsfulfilledPercent) *
                          (AveragePriceMap[WealthType] * EconomyParams.SingleUnitPriceMulti);
  }
  float MGenMulti = (ActualTotalMoney / BaseTotalMoney);
  check(MGenMulti > 0);
  for (auto& Pop : AllCustomerPops) Pop.EconData.MGen *= MGenMulti;
  for (auto& Pop : PopEconGenDataArray) Pop.MGen *= MGenMulti;

  // * Calc BoughtToPriceMulti.
  for (auto& Item : EconItems)
    Item.BoughtToPriceMulti = (Item.CurrentPrice / (AverageNeedsMap[Item.ItemWealthType] * NeedsfulfilledPercent)) *
                              EconomyParams.SingleUnitPriceMulti;

  // * Setting WealthTypePricesMap.
  for (auto WealthType : TEnumRange<EItemWealthType>()) WealthTypePricesMap.Add(WealthType, 0);
  for (auto WealthType : TEnumRange<EItemWealthType>())
    WealthTypePricesMap[WealthType] = (AveragePriceMap[WealthType] * EconomyParams.SingleUnitPriceMulti);

  BasePopTypeToEconomyTable = nullptr;
  CustomerPopDataTable = nullptr;
  AllItemsTable = nullptr;
}