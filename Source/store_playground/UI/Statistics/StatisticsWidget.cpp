#include "StatisticsWidget.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UStatisticsWidget::RefreshUI() { check(StatisticsGenRef); }