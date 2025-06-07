#include "InventoryViewWidget.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/UI/Inventory/ItemsWidget.h"
#include "store_playground/UI/Components/RightSlideWidget.h"
#include "store_playground/UI/Components/LeftSlideWidget.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Components/ControlTextWidget.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "InventoryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UInventoryViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SortByMarketPriceButton->ControlButton->OnClicked.AddDynamic(this, &UInventoryViewWidget::SortByMarketPrice);
  SortByNameButton->ControlButton->OnClicked.AddDynamic(this, &UInventoryViewWidget::SortByName);
  BackButton->ControlButton->OnClicked.AddDynamic(this, &UInventoryViewWidget::Back);
}

void UInventoryViewWidget::SortByMarketPrice() {
  ItemsWidget->SortItems(
      {.SortType = ESortType::Price,
       .bReverse = ItemsWidget->SortData.SortType == ESortType::Price ? !ItemsWidget->SortData.bReverse : false});

  SortByMarketPriceButton->ControlButton->SetBackgroundColor(FColor::FromHex("6A8DFFFF"));
  SortByNameButton->ControlButton->SetBackgroundColor(FColor::FromHex("F7F7F7FF"));
}
void UInventoryViewWidget::SortByName() {
  ItemsWidget->SortItems(
      {.SortType = ESortType::Name,
       .bReverse = ItemsWidget->SortData.SortType == ESortType::Name ? !ItemsWidget->SortData.bReverse : false});

  SortByNameButton->ControlButton->SetBackgroundColor(FColor::FromHex("6A8DFFFF"));
  SortByMarketPriceButton->ControlButton->SetBackgroundColor(FColor::FromHex("F7F7F7FF"));
}
void UInventoryViewWidget::Back() {
  check(CloseWidgetFunc);
  CloseWidgetFunc();
}

void UInventoryViewWidget::RefreshUI() {
  check(Store && MarketEconomy);

  MoneySlideWidget->SlideText->SetText(FText::FromString(FString::Printf(TEXT("Money: %.0f¬"), Store->Money)));

  ItemsWidget->RefreshUI();

  float TotalItemsValue = 0.0f;
  for (const UItemBase* Item : ItemsWidget->SortedItems) {
    float MarketPrice = MarketEconomy->GetMarketPrice(Item->ItemID);
    TotalItemsValue += Item->Quantity * MarketPrice;
  }
  ItemsValueSlideWidget->SlideText->SetText(FText::FromString(FString::Printf(TEXT("Value: %.0f¬"), TotalItemsValue)));
}

void UInventoryViewWidget::InitUI(FInputActions InputActions,
                                  const AStore* _Store,
                                  const AMarketEconomy* _MarketEconomy,
                                  const UInventoryComponent* InventoryC,
                                  std::function<void()> _CloseWidgetFunc) {
  check(_Store && _MarketEconomy && InventoryC);

  Store = _Store;
  MarketEconomy = _MarketEconomy;

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

  ItemsValueSlideWidget->RightSlideText->SetText(FText::FromString(""));

  ItemsWidget->InitUI(InventoryC, "Bought At",
                      [this](FName ItemID) -> float { return MarketEconomy->GetMarketPrice(ItemID); });

  SortByMarketPriceButton->ActionText->SetText(FText::FromString("Sort - Market Price"));
  SortByNameButton->ActionText->SetText(FText::FromString("Sort - Name"));

  BackButton->ActionText->SetText(FText::FromString("Back"));

  CloseWidgetFunc = _CloseWidgetFunc;

  // TODO:
  // SortByMarketPriceButton->ControlTextWidget->CommonActionWidget->SetEnhancedInputAction(InputActions.BuildModeAction);
}