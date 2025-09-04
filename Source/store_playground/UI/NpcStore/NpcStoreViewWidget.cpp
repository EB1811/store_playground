#include "NpcStoreViewWidget.h"
#include "CoreFwd.h"
#include "Logging/LogVerbosity.h"
#include "store_playground/UI/NpcStore/TradeConfirmWidget.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/UI/Inventory/ItemsWidget.h"
#include "store_playground/UI/Components/RightSlideWidget.h"
#include "store_playground/UI/Components/LeftSlideWidget.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Components/ControlTextWidget.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UNpcStoreViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SortByPriceButton->ControlButton->OnClicked.AddDynamic(this, &UNpcStoreViewWidget::SortByPrice);
  SortByNameButton->ControlButton->OnClicked.AddDynamic(this, &UNpcStoreViewWidget::SortByName);
  TradeButton->ControlButton->OnClicked.AddDynamic(this, &UNpcStoreViewWidget::Trade);
  SwitchTradeTypeButton->ControlButton->OnClicked.AddDynamic(this, &UNpcStoreViewWidget::SwitchTradeType);
  BackButton->ControlButton->OnClicked.AddDynamic(this, &UNpcStoreViewWidget::Back);

  SetupUIActionable();
  SetupUIBehaviour();
}

void UNpcStoreViewWidget::TradeConfirmed(int32 Quantity) {
  check(bIsConfirming);

  bool bSuccess = false;
  switch (TradeType) {
    case ETradeType::Buy: {
      if (!ItemsWidget->SelectedItem) return;
      auto ItemToBuy = StoreInventory->ItemsArray.FindByPredicate(
          [this](const UItemBase* Item) { return Item->UniqueItemID == ItemsWidget->SelectedItem->UniqueItemID; });
      check(ItemToBuy);

      bSuccess = Market->BuyItem(NpcStoreC, StoreInventory, PlayerInventory, Store, *ItemToBuy, Quantity);
      UE_LOG(LogTemp, Log, TEXT("TradeConfirmed -> Success: %s"), bSuccess ? TEXT("true") : TEXT("false"));
      break;
    }
    case ETradeType::Sell: {
      if (!ItemsWidget->SelectedItem) return;
      auto ItemToSell = PlayerInventory->ItemsArray.FindByPredicate(
          [this](const UItemBase* Item) { return Item->UniqueItemID == ItemsWidget->SelectedItem->UniqueItemID; });
      check(ItemToSell);

      bSuccess = Market->SellItem(NpcStoreC, StoreInventory, PlayerInventory, Store, *ItemToSell, Quantity);
      UE_LOG(LogTemp, Log, TEXT("TradeConfirmed -> Success: %s"), bSuccess ? TEXT("true") : TEXT("false"));
      break;
    }
    default: checkNoEntry();
  }

  TradeConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);

  bIsConfirming = false;
  RefreshUI();

  UGameplayStatics::PlaySound2D(this, TradeConfirmSound, 1.0f);
}

void UNpcStoreViewWidget::SortByPrice() {
  if (bIsConfirming) return;

  ItemsWidget->SortItems(
      {.SortType = ESortType::Price,
       .bReverse = ItemsWidget->SortData.SortType == ESortType::Price ? !ItemsWidget->SortData.bReverse : false});

  SortByPriceButton->SetActiveStyle(true);
  SortByNameButton->SetActiveStyle(false);

  UGameplayStatics::PlaySound2D(this, SortByPriceButton->ClickedSound, 1.0f);
}
void UNpcStoreViewWidget::SortByName() {
  if (bIsConfirming) return;

  ItemsWidget->SortItems(
      {.SortType = ESortType::Name,
       .bReverse = ItemsWidget->SortData.SortType == ESortType::Name ? !ItemsWidget->SortData.bReverse : false});

  SortByPriceButton->SetActiveStyle(false);
  SortByNameButton->SetActiveStyle(true);

  UGameplayStatics::PlaySound2D(this, SortByPriceButton->ClickedSound, 1.0f);
}

void UNpcStoreViewWidget::Trade() {
  if (bIsConfirming) return TradeConfirmWidget->ConfirmTrade();
  if (ItemsWidget->SelectedItem == nullptr) return;

  std::function<float()> ShowPriceFunc = nullptr;
  if (TradeType == ETradeType::Buy)
    ShowPriceFunc = [this]() -> float {
      return Market->GetNpcStoreSellPrice(NpcStoreC, ItemsWidget->SelectedItem->ItemID);
    };
  else
    ShowPriceFunc = [this]() -> float {
      return Market->GetNpcStoreBuyPrice(NpcStoreC, ItemsWidget->SelectedItem->ItemID);
    };
  auto ConfirmTradeFunc = [this](int32 Quantity) { return TradeConfirmed(Quantity); };
  auto BackFunc = [this]() {
    TradeConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);
    bIsConfirming = false;
  };

  TradeConfirmWidget->InitUI(TradeType, ItemsWidget->SelectedItem->TextData.Name, ItemsWidget->SelectedItem->Quantity,
                             Store->Money, ShowPriceFunc, ConfirmTradeFunc, BackFunc);
  TradeConfirmWidget->RefreshUI();

  bIsConfirming = true;
  TradeConfirmWidget->SetVisibility(ESlateVisibility::Visible);

  UGameplayStatics::PlaySound2D(this, GeneralTradeSound, 1.0f);
}

void UNpcStoreViewWidget::SwitchTradeType() {
  if (bIsConfirming) return;

  TradeType = TradeType == ETradeType::Buy ? ETradeType::Sell : ETradeType::Buy;
  switch (TradeType) {
    case ETradeType::Buy:
      ItemsWidget->InitUI(
          StoreInventory, StatisticsGen, "Selling At",
          [this](FName ItemID) -> float { return MarketEconomy->GetMarketPrice(ItemID); },
          [this](FName ItemID) -> float { return Market->GetNpcStoreSellPrice(NpcStoreC, ItemID); });
      if (ItemsWidget->SortData.SortType == ESortType::None) SortByPrice();
      break;
    case ETradeType::Sell:
      ItemsWidget->InitUI(
          PlayerInventory, StatisticsGen, "Buying At",
          [this](FName ItemID) -> float { return MarketEconomy->GetMarketPrice(ItemID); },
          [this](FName ItemID) -> float { return Market->GetNpcStoreBuyPrice(NpcStoreC, ItemID); });
      if (ItemsWidget->SortData.SortType == ESortType::None) SortByPrice();
      break;
    default: checkNoEntry();
  }

  RefreshUI();

  UGameplayStatics::PlaySound2D(this, GeneralTradeSound, 1.0f);
}

void UNpcStoreViewWidget::Back() {
  if (bIsConfirming) return TradeConfirmWidget->Back();

  CloseWidgetFunc();
}

void UNpcStoreViewWidget::RefreshUI() {
  check(Store && MarketEconomy);

  MoneySlideWidget->SlideText->SetText(FText::FromString(FString::Printf(TEXT("Money: %.0f¬"), Store->Money)));
  TradeButton->ActionText->SetText(FText::FromString(TradeType == ETradeType::Buy ? "Buy" : "Sell"));
  SwitchTradeTypeButton->ActionText->SetText(
      FText::FromString(TradeType == ETradeType::Buy ? "Switch to Sell" : "Switch to Buy"));

  ItemsWidget->RefreshUI();
}

void UNpcStoreViewWidget::InitUI(FInUIInputActions InUIInputActions,
                                 const class AMarketEconomy* _MarketEconomy,
                                 const class AMarket* _Market,
                                 const class AStatisticsGen* _StatisticsGen,
                                 class AStore* _Store,
                                 class UNpcStoreComponent* _NpcStoreC,
                                 class UInventoryComponent* StoreInventoryC,
                                 class UInventoryComponent* PlayerInventoryC,
                                 std::function<void()> _CloseWidgetFunc) {
  check(_MarketEconomy && _StatisticsGen && _Market && _Store && _NpcStoreC && PlayerInventoryC && StoreInventoryC &&
        _CloseWidgetFunc);

  TradeConfirmWidget->SetVisibility(ESlateVisibility::Collapsed);

  MarketEconomy = _MarketEconomy;
  Market = _Market;
  StatisticsGen = _StatisticsGen;

  TradeType = ETradeType::Buy;
  Store = _Store;
  NpcStoreC = _NpcStoreC;
  StoreInventory = StoreInventoryC;
  PlayerInventory = PlayerInventoryC;
  bIsConfirming = false;

  TArray<FTopBarTab> TopBarTabs = {
      FTopBarTab{FText::FromString("All")},
  };
  for (auto Type : TEnumRange<EItemType>()) TopBarTabs.Add(FTopBarTab{UEnum::GetDisplayValueAsText(Type)});
  auto TabSelectedFunc = [this](FText TabText) {
    if (TabText.ToString() == "All") {
      ItemsWidget->FilterData = {.ItemType = EItemType::Weapon, .bFilterByType = false};
    } else {
      EItemType ItemType = EItemType::Weapon;
      for (auto Type : TEnumRange<EItemType>()) {
        if (UEnum::GetDisplayValueAsText(Type).EqualTo(TabText)) {
          ItemType = Type;
          break;
        }
      }

      ItemsWidget->FilterData = {.ItemType = ItemType, .bFilterByType = true};
    }
    ItemsWidget->RefreshUI();
  };
  MenuHeaderWidget->SetComponentUI(TopBarTabs, TabSelectedFunc);

  MoneySlideWidget->SlideText->SetText(FText::FromString(FString::Printf(TEXT("Money: %.0f¬"), Store->Money)));
  MoneySlideWidget->RightSlideText->SetText(FText::FromString(""));

  ItemsWidget->InitUI(
      StoreInventory, StatisticsGen, "Selling At",
      [this](FName ItemID) -> float { return MarketEconomy->GetMarketPrice(ItemID); },
      [this](FName ItemID) -> float { return Market->GetNpcStoreSellPrice(NpcStoreC, ItemID); });
  if (ItemsWidget->SortData.SortType == ESortType::None) SortByPrice();

  SortByPriceButton->ActionText->SetText(FText::FromString("Sort - Price"));
  SortByNameButton->ActionText->SetText(FText::FromString("Sort - Name"));
  BackButton->ActionText->SetText(FText::FromString("Back"));

  CloseWidgetFunc = _CloseWidgetFunc;

  TradeButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.AdvanceUIAction);
  SortByPriceButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton1Action);
  SortByNameButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton2Action);
  SwitchTradeTypeButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton4Action);
  BackButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.RetractUIAction);

  TradeConfirmWidget->ConfirmTradeButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.AdvanceUIAction);
  TradeConfirmWidget->BackButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.RetractUIAction);
}

void UNpcStoreViewWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { Trade(); };
  UIActionable.DirectionalInput = [this](FVector2D Direction) {
    if (bIsConfirming) return TradeConfirmWidget->ChangeQuantity(Direction.Y);

    ItemsWidget->SelectNextItem(Direction);
  };
  UIActionable.SideButton1 = [this]() { SortByPrice(); };
  UIActionable.SideButton2 = [this]() { SortByName(); };
  UIActionable.SideButton4 = [this]() { SwitchTradeType(); };
  UIActionable.CycleLeft = [this]() {
    if (bIsConfirming) return;
    MenuHeaderWidget->CycleLeft();
  };
  UIActionable.CycleRight = [this]() {
    if (bIsConfirming) return;
    MenuHeaderWidget->CycleRight();
  };
  UIActionable.RetractUI = [this]() { Back(); };
  UIActionable.QuitUI = [this]() { CloseWidgetFunc(); };
}

void UNpcStoreViewWidget::SetupUIBehaviour() {
  UIBehaviour.ShowAnim = ShowAnim;
  UIBehaviour.HideAnim = HideAnim;
  UIBehaviour.OpenSound = OpenSound;
  UIBehaviour.HideSound = HideSound;
}