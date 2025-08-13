#include "StockDisplayViewWidget.h"
#include "CoreFwd.h"
#include "Logging/LogVerbosity.h"
#include "UObject/Class.h"
#include "store_playground/UI/NpcStore/TradeConfirmWidget.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Store/StockDisplayComponent.h"
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

void UStockDisplayViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SortByMarketPriceButton->ControlButton->OnClicked.AddDynamic(this, &UStockDisplayViewWidget::SortByMarketPrice);
  SortByNameButton->ControlButton->OnClicked.AddDynamic(this, &UStockDisplayViewWidget::SortByName);
  AddOrTakeButton->ControlButton->OnClicked.AddDynamic(this, &UStockDisplayViewWidget::AddOrTake);
  SwitchViewTypeButton->ControlButton->OnClicked.AddDynamic(this, &UStockDisplayViewWidget::SwitchViewType);
  BackButton->ControlButton->OnClicked.AddDynamic(this, &UStockDisplayViewWidget::Back);

  SetupUIActionable();
  SetupUIBehaviour();
}

void UStockDisplayViewWidget::SortByMarketPrice() {
  ItemsWidget->SortItems(
      {.SortType = ESortType::Price,
       .bReverse = ItemsWidget->SortData.SortType == ESortType::Price ? !ItemsWidget->SortData.bReverse : false});

  SortByMarketPriceButton->SetActiveStyle(true);
  SortByNameButton->SetActiveStyle(false);

  UGameplayStatics::PlaySound2D(this, SortByMarketPriceButton->ClickedSound, 1.0f);
}
void UStockDisplayViewWidget::SortByName() {
  ItemsWidget->SortItems(
      {.SortType = ESortType::Name,
       .bReverse = ItemsWidget->SortData.SortType == ESortType::Name ? !ItemsWidget->SortData.bReverse : false});

  SortByMarketPriceButton->SetActiveStyle(false);
  SortByNameButton->SetActiveStyle(true);

  UGameplayStatics::PlaySound2D(this, SortByMarketPriceButton->ClickedSound, 1.0f);
}

void UStockDisplayViewWidget::AddOrTake() {
  switch (StockDisplayViewType) {
    case EStockDisplayViewType::Player: {
      if (DisplayInventory->ItemsArray.Num() > 0) {
        auto StockItem = DisplayInventory->ItemsArray[0];
        TransferItem(DisplayInventory, PlayerInventory, StockItem);
      }
      check(DisplayInventory->ItemsArray.Num() <= 0);

      if (!ItemsWidget->SelectedItem) return;
      auto ItemToAdd = PlayerInventory->ItemsArray.FindByPredicate(
          [this](const UItemBase* Item) { return Item->UniqueItemID == ItemsWidget->SelectedItem->UniqueItemID; });
      check(ItemToAdd);

      if (TransferItem(PlayerInventory, DisplayInventory, *ItemToAdd).bSuccess) {
        // ? Add to function in store?
        StockDisplayC->SetDisplaySprite(ItemsWidget->SelectedItem->AssetData.Sprite);
        Store->InitStockDisplays();

        UGameplayStatics::PlaySound2D(this, AddOrTakeSound, 1.0f);

        return CloseWidgetFunc();
      }
      break;
    }
    case EStockDisplayViewType::Display: {
      if (!ItemsWidget->SelectedItem) return;
      auto ItemToAddRemove = DisplayInventory->ItemsArray.FindByPredicate(
          [this](const UItemBase* Item) { return Item->UniqueItemID == ItemsWidget->SelectedItem->UniqueItemID; });
      check(ItemToAddRemove);

      if (TransferItem(DisplayInventory, PlayerInventory, *ItemToAddRemove).bSuccess) {
        // ? Add to function in store?
        StockDisplayC->ClearDisplaySprite();
        Store->InitStockDisplays();

        UGameplayStatics::PlaySound2D(this, AddOrTakeSound, 1.0f);

        return CloseWidgetFunc();
      }
      break;
    }
    default: checkNoEntry();
  }
}

void UStockDisplayViewWidget::SwitchViewType() {
  StockDisplayViewType = StockDisplayViewType == EStockDisplayViewType::Player ? EStockDisplayViewType::Display
                                                                               : EStockDisplayViewType::Player;
  switch (StockDisplayViewType) {
    case EStockDisplayViewType::Player:
      ItemsWidget->InitUI(PlayerInventory, "Bought At",
                          [this](FName ItemID) -> float { return MarketEconomy->GetMarketPrice(ItemID); });
      break;
    case EStockDisplayViewType::Display:
      ItemsWidget->InitUI(DisplayInventory, "Bought At",
                          [this](FName ItemID) -> float { return MarketEconomy->GetMarketPrice(ItemID); });
      break;
    default: checkNoEntry();
  }

  RefreshUI();
  UGameplayStatics::PlaySound2D(this, SwtichSound, 1.0f);
}

void UStockDisplayViewWidget::Back() { CloseWidgetFunc(); }

void UStockDisplayViewWidget::RefreshUI() {
  MenuHeaderWidget->SetTitle(UEnum::GetDisplayValueAsText(StockDisplayViewType));
  AddOrTakeButton->ActionText->SetText(
      FText::FromString(StockDisplayViewType == EStockDisplayViewType::Player ? "Add" : "Take"));
  SwitchViewTypeButton->ActionText->SetText(
      FText::FromString(StockDisplayViewType == EStockDisplayViewType::Player ? "Show Display" : "Show Inventory"));

  ItemsWidget->RefreshUI();
}

void UStockDisplayViewWidget::InitUI(FInUIInputActions InUIInputActions,
                                     const class AMarketEconomy* _MarketEconomy,
                                     class AStore* _Store,
                                     class UStockDisplayComponent* _StockDisplayC,
                                     class UInventoryComponent* DisplayInventoryC,
                                     class UInventoryComponent* PlayerInventoryC,
                                     std::function<void()> _CloseWidgetFunc) {
  check(_MarketEconomy && _Store && _StockDisplayC && DisplayInventoryC && PlayerInventoryC && _CloseWidgetFunc);

  MarketEconomy = _MarketEconomy;
  Store = _Store;
  StockDisplayC = _StockDisplayC;
  DisplayInventory = DisplayInventoryC;
  PlayerInventory = PlayerInventoryC;

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

  SortByMarketPriceButton->ActionText->SetText(FText::FromString("Sort - Price"));
  SortByMarketPriceButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton1Action);
  SortByNameButton->ActionText->SetText(FText::FromString("Sort - Name"));
  SortByNameButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton2Action);
  BackButton->ActionText->SetText(FText::FromString("Back"));
  BackButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.RetractUIAction);

  AddOrTakeButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.AdvanceUIAction);
  SwitchViewTypeButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton4Action);

  CloseWidgetFunc = _CloseWidgetFunc;

  if (DisplayInventory->ItemsArray.Num() > 0) {
    StockDisplayViewType = EStockDisplayViewType::Display;
    ItemsWidget->InitUI(DisplayInventory, "Bought At",
                        [this](FName ItemID) -> float { return MarketEconomy->GetMarketPrice(ItemID); });
  } else {
    StockDisplayViewType = EStockDisplayViewType::Player;
    ItemsWidget->InitUI(PlayerInventory, "Bought At",
                        [this](FName ItemID) -> float { return MarketEconomy->GetMarketPrice(ItemID); });
  }
}

void UStockDisplayViewWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { AddOrTake(); };
  UIActionable.DirectionalInput = [this](FVector2D Direction) { ItemsWidget->SelectNextItem(Direction); };
  UIActionable.SideButton1 = [this]() { SortByMarketPrice(); };
  UIActionable.SideButton2 = [this]() { SortByName(); };
  UIActionable.SideButton4 = [this]() { SwitchViewType(); };
  UIActionable.CycleLeft = [this]() { MenuHeaderWidget->CycleLeft(); };
  UIActionable.CycleRight = [this]() { MenuHeaderWidget->CycleRight(); };
  UIActionable.RetractUI = [this]() { Back(); };
  UIActionable.QuitUI = [this]() { CloseWidgetFunc(); };
}

void UStockDisplayViewWidget::SetupUIBehaviour() {
  UIBehaviour.ShowAnim = ShowAnim;
  UIBehaviour.HideAnim = HideAnim;
  UIBehaviour.OpenSound = OpenSound;
  UIBehaviour.HideSound = HideSound;
}