
#include "StatisticsGen.h"
#include "Containers/AllowShrinking.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Inventory/InventoryComponent.h"
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
  StoreStatistics.StoreStockValueHistory.Reserve(StatisticsGenParams.MaxHistoryCount);
}

void AStatisticsGen::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AStatisticsGen::ItemDeal(const FItemDeal ItemDeal) { TodaysItemDeals.Add(ItemDeal); }
void AStatisticsGen::StoreMoneySpent(float Amount) { TodaysStoreMoneyActivity.AllExpenses += Amount; }
void AStatisticsGen::StoreMoneyGained(float Amount) { TodaysStoreMoneyActivity.AllIncome += Amount; }

auto AStatisticsGen::CalcTodaysStoreProfit() const -> float {
  return TodaysStoreMoneyActivity.AllIncome - TodaysStoreMoneyActivity.AllExpenses;
}

auto AStatisticsGen::CalcTotalStoreStockValue() const -> float {
  float TotalValue = 0.0f;
  for (const FStockItem& StockItem : Store->StoreStockItems) {
    const FEconItem* EconItem = MarketEconomy->EconItems.FindByPredicate(
        [StockItem](const FEconItem& EconItem) { return EconItem.ItemID == StockItem.Item->ItemID; });
    check(EconItem);

    TotalValue += EconItem->CurrentPrice;
  }

  return TotalValue;
}

void AStatisticsGen::CalcDayStatistics() {
  UE_LOG(LogTemp, Warning, TEXT("StatisticsGen: Calculating day statistics."));
  StoreStatistics.ProfitHistory.Add(CalcTodaysStoreProfit());
  StoreStatistics.TotalProfitToDate += CalcTodaysStoreProfit();
  StoreStatistics.RevenueHistory.Add(TodaysStoreMoneyActivity.AllIncome);
  StoreStatistics.TotalRevenueToDate += TodaysStoreMoneyActivity.AllIncome;
  StoreStatistics.StoreStockValueHistory.Add(CalcTotalStoreStockValue());

  if (StoreStatistics.CurrentHistoryCount < StatisticsGenParams.MaxHistoryCount) {
    StoreStatistics.CurrentHistoryCount++;
  } else {
    StoreStatistics.ProfitHistory.RemoveAt(0, EAllowShrinking::No);
    StoreStatistics.RevenueHistory.RemoveAt(0, EAllowShrinking::No);
    StoreStatistics.StoreStockValueHistory.RemoveAt(0, EAllowShrinking::No);
  }

  TodaysItemDeals.Empty();
  TodaysStoreMoneyActivity.AllExpenses = 0.0f;
  TodaysStoreMoneyActivity.AllIncome = 0.0f;
}