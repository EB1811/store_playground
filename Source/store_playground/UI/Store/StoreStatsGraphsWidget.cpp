#include "StoreStatsGraphsWidget.h"
#include "store_playground/Store/Store.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Market/Market.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/Ability/AbilityManager.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Store/StoreDetailCardWidget.h"
#include "store_playground/UI/Store/StoreMonetaryDetailCardWidget.h"
#include "store_playground/UI/Graph/StoreStatsGraphWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UStoreStatsGraphsWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

void UStoreStatsGraphsWidget::RefreshUI() {
  if (DayProfitGraphWidget->StatsHistory.Num() < 2 ||
      (StatisticsGen->StoreStatistics.ProfitHistory.Last() != DayProfitGraphWidget->StatsHistory.Last())) {
    DayProfitGraphWidget->InitUI(StatisticsGen->StoreStatistics.ProfitHistory);
    DayProfitGraphWidget->CreateStoreStatsGraph();
  }

  NetWorthGraphWidget->InitUI(StatisticsGen->StoreStatistics.NetWorthHistory);
  NetWorthGraphWidget->CreateStoreStatsGraph();
}

void UStoreStatsGraphsWidget::InitUI(const class AStatisticsGen* _StatisticsGen) {
  check(_StatisticsGen);

  StatisticsGen = _StatisticsGen;
}