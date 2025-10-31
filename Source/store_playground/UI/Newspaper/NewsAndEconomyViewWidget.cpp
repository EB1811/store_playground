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

  SetupUIActionable();
  SetupUIBehaviour();
}

void UNewsAndEconomyViewWidget::SwitchTab(ENewsAndEconomyViewTab Tab) {
  if (ActiveTab == Tab) return;

  ActiveTab = Tab;
  switch (ActiveTab) {
    case ENewsAndEconomyViewTab::Articles:
      NewspaperWidget->SetVisibility(ESlateVisibility::Visible);
      EconomyDetailsWidget->SetVisibility(ESlateVisibility::Collapsed);
      break;
    case ENewsAndEconomyViewTab::Economy:
      NewspaperWidget->SetVisibility(ESlateVisibility::Collapsed);
      EconomyDetailsWidget->SetVisibility(ESlateVisibility::Visible);
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

void UNewsAndEconomyViewWidget::InitUI(FInUIInputActions InUIInputActions,
                                       const ADayManager* _DayManager,
                                       const AMarketEconomy* _MarketEconomy,
                                       const class AStatisticsGen* _StatisticsGen,
                                       ANewsGen* _NewsGen,
                                       std::function<void()> _CloseWidgetFunc) {
  check(_DayManager && _NewsGen && _MarketEconomy && _StatisticsGen && _CloseWidgetFunc);

  TArray<FTopBarTab> TopBarTabs = {};
  for (auto Tab : TEnumRange<ENewsAndEconomyViewTab>()) TopBarTabs.Add(FTopBarTab{GetNewsAndEconomyViewTabText(Tab)});
  auto TabSelectedFunc = [this](FText TabText) {
    ENewsAndEconomyViewTab SelectedTab = ENewsAndEconomyViewTab::Articles;
    for (auto Tab : TEnumRange<ENewsAndEconomyViewTab>()) {
      if (GetNewsAndEconomyViewTabText(Tab).EqualTo(TabText)) {
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
  EconomyDetailsWidget->InitUI(_MarketEconomy, _StatisticsGen);

  BackButton->ActionText->SetText(FText::FromString("Back"));
  BackButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.RetractUIAction);

  CloseWidgetFunc = _CloseWidgetFunc;
}

void UNewsAndEconomyViewWidget::SetupUIActionable() {
  UIActionable.CycleLeft = [this]() { MenuHeaderWidget->CycleLeft(); };
  UIActionable.CycleRight = [this]() { MenuHeaderWidget->CycleRight(); };
  UIActionable.RetractUI = [this]() { Back(); };
  UIActionable.QuitUI = [this]() { CloseWidgetFunc(); };
}
void UNewsAndEconomyViewWidget::SetupUIBehaviour() {
  UIBehaviour.ShowAnim = ShowAnim;
  UIBehaviour.HideAnim = HideAnim;
  UIBehaviour.OpenSound = OpenSound;
  UIBehaviour.HideSound = HideSound;
}