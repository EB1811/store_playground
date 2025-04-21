#include "StatisticsWidget.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UStatisticsWidget::RefreshUI() {
  check(StatisticsGenRef);

  TotalProfitText->SetText(FText::FromString(
      FString::SanitizeFloat((FMath::RoundToInt32(StatisticsGenRef->StoreStatistics.TotalProfitToDate)))));
  StoreStockValueText->SetText(
      FText::FromString(FString::SanitizeFloat((FMath::RoundToInt32(StatisticsGenRef->CalcTotalStoreStockValue())))));
}