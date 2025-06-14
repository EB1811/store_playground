#include "NewsAndEconomyViewWidget.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Components/ControlTextWidget.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "store_playground/UI/Newspaper/EconomyDetailsWidget.h"
#include "store_playground/UI/Newspaper/NewspaperWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UNewsAndEconomyViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  BackButton->ControlButton->OnClicked.AddDynamic(this, &UNewsAndEconomyViewWidget::Back);
}

void UNewsAndEconomyViewWidget::SwitchTab(ENewsAndEconomyViewTab Tab) {
  if (ActiveTab == Tab) return;

  ActiveTab = Tab;
  switch (ActiveTab) {
    case ENewsAndEconomyViewTab::Articles:
      NewspaperWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
      EconomyDetailsWidget->SetVisibility(ESlateVisibility::Collapsed);
      break;
    case ENewsAndEconomyViewTab::Economy:
      NewspaperWidget->SetVisibility(ESlateVisibility::Collapsed);
      EconomyDetailsWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
      break;
    default: checkNoEntry();
  }
}

void UNewsAndEconomyViewWidget::Back() { CloseWidgetFunc(); }

void UNewsAndEconomyViewWidget::RefreshUI() {
  switch (ActiveTab) {
    case ENewsAndEconomyViewTab::Articles: NewspaperWidget->RefreshUI(); break;
    case ENewsAndEconomyViewTab::Economy: EconomyDetailsWidget->RefreshUI(); break;
    default: checkNoEntry();
  }
}

void UNewsAndEconomyViewWidget::InitUI(FInputActions InputActions,
                                       const ADayManager* _DayManager,
                                       const AMarketEconomy* _MarketEconomy,
                                       ANewsGen* _NewsGen,
                                       std::function<void()> _CloseWidgetFunc) {
  check(_DayManager && _NewsGen && _MarketEconomy && _CloseWidgetFunc);

  TArray<FTopBarTab> TopBarTabs = {};
  for (auto Tab : TEnumRange<ENewsAndEconomyViewTab>()) TopBarTabs.Add(FTopBarTab{UEnum::GetDisplayValueAsText(Tab)});
  auto TabSelectedFunc = [this](FText TabText) {
    ENewsAndEconomyViewTab SelectedTab = ENewsAndEconomyViewTab::Articles;
    for (auto Tab : TEnumRange<ENewsAndEconomyViewTab>()) {
      if (UEnum::GetDisplayValueAsText(Tab).EqualTo(TabText)) {
        SelectedTab = Tab;
        break;
      }
    }
    SwitchTab(SelectedTab);
    RefreshUI();
  };
  MenuHeaderWidget->SetComponentUI(TopBarTabs, TabSelectedFunc);
  SwitchTab(ENewsAndEconomyViewTab::Articles);

  NewspaperWidget->InitUI(_DayManager, _NewsGen);
  EconomyDetailsWidget->InitUI(_MarketEconomy);

  BackButton->ActionText->SetText(FText::FromString("Back"));

  CloseWidgetFunc = _CloseWidgetFunc;
}