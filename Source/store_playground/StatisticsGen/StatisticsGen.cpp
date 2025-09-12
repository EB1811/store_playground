
#include "StatisticsGen.h"
#include "Containers/AllowShrinking.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/WorldObject/Buildable.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "store_playground/WorldObject/Buildable.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/SaveManager/SaveManager.h"

void AStatisticsGen::BeginPlay() {
  Super::BeginPlay();

  StoreStatistics.ProfitHistory.Reserve(StatisticsGenParams.DayHistoryCount);
  StoreStatistics.RevenueHistory.Reserve(StatisticsGenParams.DayHistoryCount);
  StoreStatistics.NetWorthHistory.Reserve(StatisticsGenParams.ValueHistoryCount);
}

void AStatisticsGen::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AStatisticsGen::ItemDeal(const FItemDeal ItemDeal) {
  TodaysItemDeals.Add(ItemDeal);

  UpgradeManager->ConsiderUpgradePoints();
}
void AStatisticsGen::StoreMoneySpent(float Amount) {
  TodaysStoreMoneyActivity.AllExpenses += Amount;

  UpgradeManager->ConsiderUpgradePoints();
}
void AStatisticsGen::StoreMoneyGained(float Amount) {
  TodaysStoreMoneyActivity.AllIncome += Amount;

  UpgradeManager->ConsiderUpgradePoints();
}

auto AStatisticsGen::CalcTodaysStoreProfit() const -> float {
  return TodaysStoreMoneyActivity.AllIncome - TodaysStoreMoneyActivity.AllExpenses;
}

auto AStatisticsGen::CalcNetWorth() -> float {
  float TotalValue = 0.0f;
  TotalValue += Store->Money;
  for (const auto& Item : PlayerInventoryC->ItemsArray)
    TotalValue += Item->Quantity * MarketEconomy->GetMarketPrice(Item->ItemID);
  for (const FStockItem& StockItem : Store->StoreStockItems)
    TotalValue += MarketEconomy->GetMarketPrice(StockItem.ItemId);

  CachedDetails.PlayerNetWorth = TotalValue;

  StoreStatistics.NetWorthHistory.Add(TotalValue);
  if (StoreStatistics.NetWorthHistory.Num() > StatisticsGenParams.ValueHistoryCount)
    StoreStatistics.NetWorthHistory.RemoveAt(0);

  return TotalValue;
}

void AStatisticsGen::ItemPriceChange(const FName ItemId, const float NewPrice) {
  if (!ItemStatisticsMap.Contains(ItemId)) {
    ItemStatisticsMap.Add(ItemId, {});
    ItemStatisticsMap[ItemId].PriceHistory.Reserve(StatisticsGenParams.ItemPriceHistoryCount);
  }

  ItemStatisticsMap[ItemId].ItemId = ItemId;
  ItemStatisticsMap[ItemId].PriceHistory.Add(NewPrice);

  if (ItemStatisticsMap[ItemId].PriceHistory.Num() > StatisticsGenParams.ItemPriceHistoryCount)
    ItemStatisticsMap[ItemId].PriceHistory.RemoveAt(0);
}

void AStatisticsGen::PopChange(const FName PopId, float NewPopulation, float NewGoodsBoughtPerCapita) {
  if (!PopStatisticsMap.Contains(PopId)) PopStatisticsMap.Add(PopId, {});

  PopStatisticsMap[PopId].PopulationHistory.Add(NewPopulation);
  PopStatisticsMap[PopId].TodaysPopulationChange = NewPopulation - PopStatisticsMap[PopId].PopulationHistory.Last();
  PopStatisticsMap[PopId].GoodsBoughtPerCapitaHistory.Add(NewGoodsBoughtPerCapita);

  if (PopStatisticsMap[PopId].PopulationHistory.Num() > StatisticsGenParams.ValueHistoryCount)
    PopStatisticsMap[PopId].PopulationHistory.RemoveAt(0);
  if (PopStatisticsMap[PopId].GoodsBoughtPerCapitaHistory.Num() > StatisticsGenParams.ValueHistoryCount)
    PopStatisticsMap[PopId].GoodsBoughtPerCapitaHistory.RemoveAt(0);
}

void AStatisticsGen::CalcDayStatistics() {
  UE_LOG(LogTemp, Warning, TEXT("StatisticsGen: Calculating day statistics."));
  StoreStatistics.ProfitHistory.Add(CalcTodaysStoreProfit());
  StoreStatistics.TotalProfitToDate += CalcTodaysStoreProfit();
  StoreStatistics.RevenueHistory.Add(TodaysStoreMoneyActivity.AllIncome);
  StoreStatistics.TotalRevenueToDate += TodaysStoreMoneyActivity.AllIncome;
  StoreStatistics.ExpensesHistory.Add(TodaysStoreMoneyActivity.AllExpenses);
  StoreStatistics.TotalExpensesToDate += TodaysStoreMoneyActivity.AllExpenses;

  if (StoreStatistics.CurrentHistoryCount < StatisticsGenParams.DayHistoryCount) {
    StoreStatistics.CurrentHistoryCount++;
  } else {
    StoreStatistics.ProfitHistory.RemoveAt(0);
    StoreStatistics.RevenueHistory.RemoveAt(0);
  }

  // for (auto& ItemStat : ItemStatisticsMap)
  //   if (ItemStat.Value.PriceHistory.Num() > StatisticsGenParams.MaxHistoryCount * 2)
  //     ItemStat.Value.PriceHistory.RemoveAt(0, EAllowShrinking::No);
  // for (auto& PopStat : PopStatisticsMap)
  //   if (PopStat.Value.PopulationHistory.Num() > StatisticsGenParams.MaxHistoryCount * 2)
  //     PopStat.Value.PopulationHistory.RemoveAt(0, EAllowShrinking::No);

  TodaysItemDeals.Empty();
  TodaysStoreMoneyActivity.AllExpenses = 0.0f;
  TodaysStoreMoneyActivity.AllIncome = 0.0f;
  for (auto& PopStat : PopStatisticsMap) PopStat.Value.TodaysPopulationChange = 0.0f;
}