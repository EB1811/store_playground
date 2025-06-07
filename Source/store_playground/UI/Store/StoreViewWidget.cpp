#include "StoreViewWidget.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Components/ControlTextWidget.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "store_playground/UI/Store/StoreDetailsWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UStoreViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  BackButton->ControlButton->OnClicked.AddDynamic(this, &UStoreViewWidget::Back);
}

void UStoreViewWidget::SwitchTab(EStoreViewTab Tab) {
  if (ActiveTab == Tab) return;

  ActiveTab = Tab;
  switch (ActiveTab) {
    case EStoreViewTab::Details: StoreDetailsWidget->SetVisibility(ESlateVisibility::Visible); break;
    case EStoreViewTab::Expansion: StoreDetailsWidget->SetVisibility(ESlateVisibility::Collapsed); break;
    default: checkNoEntry();
  }
}

void UStoreViewWidget::Back() { CloseWidgetFunc(); }

void UStoreViewWidget::RefreshUI() {
  switch (ActiveTab) {
    case EStoreViewTab::Details: StoreDetailsWidget->RefreshUI(); break;
    case EStoreViewTab::Expansion: break;
    default: checkNoEntry();
  }
}

void UStoreViewWidget::InitUI(FInputActions InputActions,
                              const ADayManager* DayManager,
                              const AStorePhaseManager* StorePhaseManager,
                              const AMarketEconomy* MarketEconomy,
                              const AMarket* Market,
                              const AStatisticsGen* StatisticsGen,
                              const AUpgradeManager* UpgradeManager,
                              const AAbilityManager* AbilityManager,
                              const UInventoryComponent* PlayerInventoryC,
                              AStore* Store,
                              AStoreExpansionManager* StoreExpansionManager,
                              std::function<void()> _CloseWidgetFunc) {
  check(DayManager && StorePhaseManager && MarketEconomy && Market && StatisticsGen && UpgradeManager &&
        AbilityManager && PlayerInventoryC && Store && StoreExpansionManager && _CloseWidgetFunc);

  TArray<FTopBarTab> TopBarTabs = {};
  for (auto Tab : TEnumRange<EStoreViewTab>()) TopBarTabs.Add(FTopBarTab{UEnum::GetDisplayValueAsText(Tab)});
  auto TabSelectedFunc = [this](FText TabText) {
    EStoreViewTab SelectedTab = EStoreViewTab::Details;
    for (auto Tab : TEnumRange<EStoreViewTab>()) {
      if (UEnum::GetDisplayValueAsText(Tab).EqualTo(TabText)) {
        SelectedTab = Tab;
        break;
      }
    }
    SwitchTab(SelectedTab);
    RefreshUI();
  };
  MenuHeaderWidget->SetComponentUI(TopBarTabs, TabSelectedFunc);
  ActiveTab = EStoreViewTab::Details;

  StoreDetailsWidget->InitUI(DayManager, StorePhaseManager, MarketEconomy, Market, StatisticsGen, UpgradeManager,
                             AbilityManager, PlayerInventoryC, Store);

  BackButton->ActionText->SetText(FText::FromString("Back"));

  CloseWidgetFunc = _CloseWidgetFunc;
}