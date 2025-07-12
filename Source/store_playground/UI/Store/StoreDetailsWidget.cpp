#include "StoreDetailsWidget.h"
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
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UStoreDetailsWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

void UStoreDetailsWidget::RefreshUI() {
  FText CurrentPhaseText = UEnum::GetDisplayValueAsText(StorePhaseManager->StorePhaseState);
  int32 CurrentDay = DayManager->CurrentDay;
  int32 NextWeekendDay = CurrentDay + (CurrentDay % DayManager->DayManagerParams.WeekendDivisor == 0
                                           ? DayManager->DayManagerParams.WeekendDivisor
                                           : DayManager->DayManagerParams.WeekendDivisor *
                                                 (1 - (CurrentDay % DayManager->DayManagerParams.WeekendDivisor)));
  DayCardWidget->InitUI(
      FText::FromString(FString::Printf(TEXT("%s - Day: %d"), *CurrentPhaseText.ToString(), CurrentDay)),
      FText::GetEmpty(), FText::FromString(DayManager->bIsWeekend ? "Weekend" : "Normal Day"),
      FText::FromString(FString::Printf(TEXT("Next Weekend: Day: %d"), NextWeekendDay)));

  int32 NextPaymentDay = DayManager->NextDayToPayDebt;
  float NextDebtAmount = DayManager->NextDebtAmount;
  DebtCardWidget->InitUI(FText::FromString("Notices"), FText::GetEmpty(),
                         FText::FromString(FString::Printf(TEXT("Next Payment: Day: %d"), NextPaymentDay)),
                         FText::FromString(FString::Printf(TEXT("Amount: %.0f¬"), NextDebtAmount)));

  float Profit = StatisticsGen->StoreStatistics.TotalProfitToDate + StatisticsGen->CalcTodaysStoreProfit();
  float Revenue = StatisticsGen->StoreStatistics.TotalRevenueToDate + StatisticsGen->TodaysStoreMoneyActivity.AllIncome;
  float Expenses =
      StatisticsGen->TodaysStoreMoneyActivity.AllExpenses + StatisticsGen->StoreStatistics.TotalExpensesToDate;
  ProfitCardWidget->InitUI(FText::FromString(FString::Printf(TEXT("Profit:  %.0f¬"), Profit)),
                           {
                               {
                                   FText::FromString("Revenue:"),
                                   Revenue,
                               },
                               {
                                   FText::FromString("Expenses:"),
                                   Expenses,
                               },
                           });

  float Money = Store->Money;
  float TotalItemsValue = 0.0f;
  float OnDisplayValue = 0.0f;
  for (const auto& Item : PlayerInventoryC->ItemsArray)
    TotalItemsValue += Item->Quantity * MarketEconomy->GetMarketPrice(Item->ItemID);
  for (const auto& StockItem : Store->StoreStockItems)
    OnDisplayValue += MarketEconomy->GetMarketPrice(StockItem.ItemId);
  NetWorthCardWidget->InitUI(
      FText::FromString(FString::Printf(TEXT("Net Worth:  %.0f¬"), Money + TotalItemsValue + OnDisplayValue)),
      {{
           FText::FromString("Money:"),
           Money,
       },
       {
           FText::FromString("Items:"),
           TotalItemsValue,
       },
       {
           FText::FromString("On Display:"),
           OnDisplayValue,
       }});

  int32 UpgradePoints = UpgradeManager->AvailableUpgradePoints;
  int32 AbilitiesAvailable = AbilityManager->GetAvailableEconEventAbilities().Num();
  UpgradesCardWidget->InitUI(FText::FromString(FString::Printf(TEXT("Upgrade Points: %d"), UpgradePoints)),
                             FText::FromString(FString::Printf(TEXT("Abilities Available: %d"), AbilitiesAvailable)),
                             FText::FromString("Leave the store during the night."), FText::GetEmpty(),
                             FText::FromString("Earn points by gaining money, speaking to npcs, and finding secrets."));
}

void UStoreDetailsWidget::InitUI(const class ADayManager* _DayManager,
                                 const class AStorePhaseManager* _StorePhaseManager,
                                 const class AMarketEconomy* _MarketEconomy,
                                 const class AMarket* _Market,
                                 const class AStatisticsGen* _StatisticsGen,
                                 const class AUpgradeManager* _UpgradeManager,
                                 const class AAbilityManager* _AbilityManager,
                                 const class UInventoryComponent* _PlayerInventoryC,
                                 class AStore* _Store) {
  check(_DayManager && _StorePhaseManager && _MarketEconomy && _Market && _StatisticsGen && _UpgradeManager &&
        _AbilityManager && _PlayerInventoryC && _Store);

  DayManager = _DayManager;
  StorePhaseManager = _StorePhaseManager;
  MarketEconomy = _MarketEconomy;
  Market = _Market;
  StatisticsGen = _StatisticsGen;
  UpgradeManager = _UpgradeManager;
  AbilityManager = _AbilityManager;
  PlayerInventoryC = _PlayerInventoryC;
  Store = _Store;
}