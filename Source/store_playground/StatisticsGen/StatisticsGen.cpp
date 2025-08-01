
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

  StoreStatistics.ProfitHistory.Reserve(StatisticsGenParams.MaxHistoryCount);
  StoreStatistics.RevenueHistory.Reserve(StatisticsGenParams.MaxHistoryCount);
  StoreStatistics.NetWorthHistory.Reserve(StatisticsGenParams.MaxHistoryCount);
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

  return TotalValue;
}

void AStatisticsGen::ItemPriceChange(const FName ItemId, const float NewPrice) {
  if (!ItemStatisticsMap.Contains(ItemId)) ItemStatisticsMap.Add(ItemId, {});

  ItemStatisticsMap[ItemId].PriceHistory.Add(NewPrice);

  if (ItemStatisticsMap[ItemId].PriceHistory.Num() > StatisticsGenParams.MaxHistoryCount * 2)
    ItemStatisticsMap[ItemId].PriceHistory.RemoveAt(0, EAllowShrinking::No);
}

void AStatisticsGen::PopChange(const FName PopId, float NewPopulation, float NewGoodsBoughtPerCapita) {
  if (!PopStatisticsMap.Contains(PopId)) PopStatisticsMap.Add(PopId, {});

  PopStatisticsMap[PopId].PopulationHistory.Add(NewPopulation);
  PopStatisticsMap[PopId].TodaysPopulationChange = NewPopulation - PopStatisticsMap[PopId].PopulationHistory.Last();
  PopStatisticsMap[PopId].GoodsBoughtPerCapitaHistory.Add(NewGoodsBoughtPerCapita);

  if (PopStatisticsMap[PopId].PopulationHistory.Num() > StatisticsGenParams.MaxHistoryCount * 2)
    PopStatisticsMap[PopId].PopulationHistory.RemoveAt(0, EAllowShrinking::No);
}

void AStatisticsGen::CalcDayStatistics() {
  UE_LOG(LogTemp, Warning, TEXT("StatisticsGen: Calculating day statistics."));
  StoreStatistics.ProfitHistory.Add(CalcTodaysStoreProfit());
  StoreStatistics.TotalProfitToDate += CalcTodaysStoreProfit();
  StoreStatistics.RevenueHistory.Add(TodaysStoreMoneyActivity.AllIncome);
  StoreStatistics.TotalRevenueToDate += TodaysStoreMoneyActivity.AllIncome;
  StoreStatistics.ExpensesHistory.Add(TodaysStoreMoneyActivity.AllExpenses);
  StoreStatistics.TotalExpensesToDate += TodaysStoreMoneyActivity.AllExpenses;
  StoreStatistics.NetWorthHistory.Add(CalcNetWorth());

  if (StoreStatistics.CurrentHistoryCount < StatisticsGenParams.MaxHistoryCount) {
    StoreStatistics.CurrentHistoryCount++;
  } else {
    StoreStatistics.ProfitHistory.RemoveAt(0, EAllowShrinking::No);
    StoreStatistics.RevenueHistory.RemoveAt(0, EAllowShrinking::No);
    StoreStatistics.NetWorthHistory.RemoveAt(0, EAllowShrinking::No);
  }

  for (auto& PopStat : PopStatisticsMap) {
    if (PopStat.Value.PopulationHistory.Num() > StatisticsGenParams.MaxHistoryCount * 5)
      PopStat.Value.PopulationHistory.RemoveAt(0, EAllowShrinking::No);
    if (PopStat.Value.GoodsBoughtPerCapitaHistory.Num() > StatisticsGenParams.MaxHistoryCount * 5)
      PopStat.Value.GoodsBoughtPerCapitaHistory.RemoveAt(0, EAllowShrinking::No);
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