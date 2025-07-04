#include "BuildableDisplayViewWidget.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Components/ControlTextWidget.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "store_playground/UI/Store/StoreDetailsWidget.h"
#include "store_playground/UI/Store/BuildableDisplayWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UBuildableDisplayViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SetupUIActionable();
}

void UBuildableDisplayViewWidget::RefreshUI() { BuildableDisplayWidget->RefreshUI(); }

void UBuildableDisplayViewWidget::InitUI(FInUIInputActions InUIInputActions,
                                         class ABuildable* Buildable,
                                         class AStore* _Store,
                                         std::function<void()> _CloseWidgetFunc) {
  check(Buildable && _Store && _CloseWidgetFunc);

  BuildableDisplayWidget->InitUI(InUIInputActions, Buildable, _Store, _CloseWidgetFunc);

  CloseWidgetFunc = _CloseWidgetFunc;
}

void UBuildableDisplayViewWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { BuildableDisplayWidget->BuildStockDisplay(); };
  UIActionable.RetractUI = [this]() { CloseWidgetFunc(); };
}