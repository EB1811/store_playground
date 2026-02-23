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
#include "store_playground/UI/Store/StoreExpansion/StoreExpansionsListWidget.h"
#include "store_playground/UI/Debt/DebtWidget.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/Level/LevelManager.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UStoreViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  BackButton->ControlButton->OnClicked.AddDynamic(this, &UStoreViewWidget::Back);

  SetupUIActionable();
  SetupUIBehaviour();
}

void UStoreViewWidget::SwitchTab(EStoreViewTab Tab) {
  ActiveTab = Tab;
  switch (ActiveTab) {
    case EStoreViewTab::Details:
      StoreDetailsWidget->SetVisibility(ESlateVisibility::Visible);
      StoreExpansionsListWidget->SetVisibility(ESlateVisibility::Collapsed);
      DebtWidget->SetVisibility(ESlateVisibility::Collapsed);
      UnlockButton->SetVisibility(ESlateVisibility::Hidden);
      break;
    case EStoreViewTab::Expansion:
      if (LevelManager->CurrentLevel != ELevel::Store) {
        ActiveTab = EStoreViewTab::Details;
        return;
      }

      StoreDetailsWidget->SetVisibility(ESlateVisibility::Collapsed);
      StoreExpansionsListWidget->SetVisibility(ESlateVisibility::Visible);
      DebtWidget->SetVisibility(ESlateVisibility::Collapsed);
      UnlockButton->SetVisibility(ESlateVisibility::Visible);
      break;
    case EStoreViewTab::Finance:
      StoreDetailsWidget->SetVisibility(ESlateVisibility::Collapsed);
      StoreExpansionsListWidget->SetVisibility(ESlateVisibility::Collapsed);
      DebtWidget->SetVisibility(ESlateVisibility::Visible);
      UnlockButton->SetVisibility(ESlateVisibility::Hidden);
      break;
    default: checkNoEntry();
  }
}

void UStoreViewWidget::Back() { CloseWidgetFunc(); }

void UStoreViewWidget::RefreshUI() {
  switch (ActiveTab) {
    case EStoreViewTab::Details: StoreDetailsWidget->RefreshUI(); break;
    case EStoreViewTab::Expansion: StoreExpansionsListWidget->RefreshUI(); break;
    case EStoreViewTab::Finance: DebtWidget->RefreshUI(); break;
    default: checkNoEntry();
  }
}

void UStoreViewWidget::InitUI(FInUIInputActions InUIInputActions,
                              const class ALevelManager* _LevelManager,
                              const ADayManager* DayManager,
                              const AStorePhaseManager* StorePhaseManager,
                              const AMarketEconomy* MarketEconomy,
                              const AMarket* Market,
                              const AUpgradeManager* UpgradeManager,
                              const AAbilityManager* AbilityManager,
                              const UInventoryComponent* PlayerInventoryC,
                              AStatisticsGen* StatisticsGen,
                              ADebtManager* DebtManager,
                              AStore* Store,
                              AStoreExpansionManager* StoreExpansionManager,
                              std::function<void()> _CloseWidgetFunc) {
  check(_LevelManager && DayManager && StorePhaseManager && MarketEconomy && Market && StatisticsGen &&
        UpgradeManager && AbilityManager && PlayerInventoryC && DebtManager && Store && StoreExpansionManager &&
        _CloseWidgetFunc);

  LevelManager = _LevelManager;

  TArray<FTopBarTab> TopBarTabs = {};
  for (auto Tab : TEnumRange<EStoreViewTab>()) {
    if (Tab == EStoreViewTab::Expansion && (LevelManager->CurrentLevel != ELevel::Store ||
                                            StorePhaseManager->StorePhaseState == EStorePhaseState::ShopOpen))
      continue;

    TopBarTabs.Add(FTopBarTab{GetStoreViewTabText(Tab)});
  }
  auto TabSelectedFunc = [this](FText TabText) {
    EStoreViewTab SelectedTab = EStoreViewTab::Details;
    for (auto Tab : TEnumRange<EStoreViewTab>()) {
      if (GetStoreViewTabText(Tab).EqualTo(TabText)) {
        SelectedTab = Tab;
        break;
      }
    }
    SwitchTab(SelectedTab);
    RefreshUI();
  };
  MenuHeaderWidget->SetComponentUI(TopBarTabs, TabSelectedFunc);
  SwitchTab(EStoreViewTab::Details);

  StoreDetailsWidget->InitUI(DayManager, StorePhaseManager, MarketEconomy, Market, DebtManager, UpgradeManager,
                             AbilityManager, PlayerInventoryC, StatisticsGen, Store);
  StoreExpansionsListWidget->InitUI(Store, StoreExpansionManager, [this]() { CloseWidgetFunc(); });
  DebtWidget->InitUI(InUIInputActions, Store, DebtManager);

  UnlockButton->SetVisibility(ESlateVisibility::Hidden);
  UnlockButton->ActionText->SetText(FText::FromString("Unlock (Hold)"));
  UnlockButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.AdvanceUIHoldAction);
  BackButton->ActionText->SetText(FText::FromString("Back"));
  BackButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.RetractUIAction);

  CloseWidgetFunc = _CloseWidgetFunc;
}

void UStoreViewWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() {
    if (ActiveTab == EStoreViewTab::Finance) DebtWidget->IssueDebt();
  };
  UIActionable.AdvanceUIHold = [this]() {
    if (ActiveTab == EStoreViewTab::Expansion) StoreExpansionsListWidget->UnlockExpansion();
  };
  UIActionable.DirectionalInput = [this](FVector2D Direction) {
    if (ActiveTab == EStoreViewTab::Expansion) StoreExpansionsListWidget->SelectNextExpansion(Direction);
    else if (ActiveTab == EStoreViewTab::Finance) DebtWidget->ChangeAmount(Direction);
  };
  UIActionable.CycleLeft = [this]() { MenuHeaderWidget->CycleLeft(); };
  UIActionable.CycleRight = [this]() { MenuHeaderWidget->CycleRight(); };
  UIActionable.RetractUI = [this]() { Back(); };
  UIActionable.QuitUI = [this]() { CloseWidgetFunc(); };
}
void UStoreViewWidget::SetupUIBehaviour() {
  UIBehaviour.ShowAnim = ShowAnim;
  UIBehaviour.HideAnim = HideAnim;
  UIBehaviour.OpenSound = OpenSound;
  UIBehaviour.HideSound = HideSound;
}