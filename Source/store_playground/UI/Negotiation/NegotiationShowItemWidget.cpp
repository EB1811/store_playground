#include "NegotiationShowItemWidget.h"
#include "CoreFwd.h"
#include "Logging/LogVerbosity.h"
#include "store_playground/UI/NpcStore/TradeConfirmWidget.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/AI/NegotiationAI.h"
#include "store_playground/UI/Inventory/ItemsWidget.h"
#include "store_playground/UI/Components/RightSlideWidget.h"
#include "store_playground/UI/Components/LeftSlideWidget.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Components/ControlTextWidget.h"
#include "store_playground/UI/Components/MenuHeaderWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UNegotiationShowItemWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SortByPriceButton->ControlButton->OnClicked.AddDynamic(this, &UNegotiationShowItemWidget::SortByPrice);
  SortByNameButton->ControlButton->OnClicked.AddDynamic(this, &UNegotiationShowItemWidget::SortByName);
  ShowButton->ControlButton->OnClicked.AddDynamic(this, &UNegotiationShowItemWidget::Show);
  RejectButton->ControlButton->OnClicked.AddDynamic(this, &UNegotiationShowItemWidget::Reject);

  SetupUIActionable();
}

void UNegotiationShowItemWidget::SortByPrice() {
  ItemsWidget->SortItems(
      {.SortType = ESortType::Price,
       .bReverse = ItemsWidget->SortData.SortType == ESortType::Price ? !ItemsWidget->SortData.bReverse : false});

  SortByPriceButton->SetActiveStyle(true);
  SortByNameButton->SetActiveStyle(false);

  UGameplayStatics::PlaySound2D(this, SortByPriceButton->ClickedSound, 1.0f);
}
void UNegotiationShowItemWidget::SortByName() {
  ItemsWidget->SortItems(
      {.SortType = ESortType::Name,
       .bReverse = ItemsWidget->SortData.SortType == ESortType::Name ? !ItemsWidget->SortData.bReverse : false});

  SortByPriceButton->SetActiveStyle(false);
  SortByNameButton->SetActiveStyle(true);

  UGameplayStatics::PlaySound2D(this, SortByPriceButton->ClickedSound, 1.0f);
}

void UNegotiationShowItemWidget::Show() {
  if (ItemsWidget->SelectedItem == nullptr) return;
  auto ItemToShow = InventoryC->ItemsArray.FindByPredicate(
      [this](const UItemBase* Item) { return Item->UniqueItemID == ItemsWidget->SelectedItem->UniqueItemID; });
  check(ItemToShow);

  ShowFunc(*ItemToShow);
}
void UNegotiationShowItemWidget::Reject() { RejectFunc(); }

void UNegotiationShowItemWidget::RefreshUI() {
  check(Store && MarketEconomy);

  MoneySlideWidget->SlideText->SetText(FText::FromString(FString::Printf(TEXT("Money: %.0f¬"), Store->Money)));

  ItemsWidget->RefreshUI();
}

void UNegotiationShowItemWidget::InitUI(FInUIInputActions InUIInputActions,
                                        const class AStore* _Store,
                                        const class AMarketEconomy* _MarketEconomy,
                                        class UInventoryComponent* _InventoryC,
                                        class UNegotiationSystem* _NegotiationSystem,
                                        std::function<void(class UItemBase*)> _ShowFunc,
                                        std::function<void()> _RejectFunc) {
  check(_Store && _MarketEconomy && _InventoryC && _NegotiationSystem && _ShowFunc && _RejectFunc);

  MarketEconomy = _MarketEconomy;
  Store = _Store;
  InventoryC = _InventoryC;
  NegotiationSystem = _NegotiationSystem;

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

  FText WantedItemTypeName = NegotiationSystem->WantedItemType.WantedItemTypeName;
  MenuHeaderWidget->SetTitle(WantedItemTypeName);

  MoneySlideWidget->SlideText->SetText(FText::FromString(FString::Printf(TEXT("Money: %.0f¬"), Store->Money)));
  MoneySlideWidget->RightSlideText->SetText(FText::FromString(""));

  ItemsWidget->InitUI(InventoryC,
                      "Bought At:", [this](FName ItemID) -> float { return MarketEconomy->GetMarketPrice(ItemID); });
  if (ItemsWidget->SortData.SortType == ESortType::None) SortByPrice();

  SortByPriceButton->ActionText->SetText(FText::FromString("Sort - Price"));
  SortByPriceButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton1Action);
  SortByNameButton->ActionText->SetText(FText::FromString("Sort - Name"));
  SortByNameButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton2Action);
  ShowButton->ActionText->SetText(FText::FromString("Show Item"));
  ShowButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.AdvanceUIAction);
  RejectButton->ActionText->SetText(FText::FromString("Reject"));
  RejectButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.RetractUIAction);

  ShowFunc = _ShowFunc;
  RejectFunc = _RejectFunc;
}

void UNegotiationShowItemWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { Show(); };
  UIActionable.DirectionalInput = [this](FVector2D Direction) { ItemsWidget->SelectNextItem(Direction); };
  UIActionable.SideButton1 = [this]() { SortByPrice(); };
  UIActionable.SideButton2 = [this]() { SortByName(); };
  UIActionable.CycleLeft = [this]() { MenuHeaderWidget->CycleLeft(); };
  UIActionable.CycleRight = [this]() { MenuHeaderWidget->CycleRight(); };
  UIActionable.RetractUI = [this]() { Reject(); };
}