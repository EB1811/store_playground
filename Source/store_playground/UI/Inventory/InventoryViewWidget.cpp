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
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UInventoryViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SortByMarketPriceButton->ControlButton->OnClicked.AddDynamic(this, &UInventoryViewWidget::SortByMarketPrice);
  SortByNameButton->ControlButton->OnClicked.AddDynamic(this, &UInventoryViewWidget::SortByName);
  BackButton->ControlButton->OnClicked.AddDynamic(this, &UInventoryViewWidget::Back);

  SetupUIActionable();
  SetupUIBehaviour();
}

void UInventoryViewWidget::SortByMarketPrice() {
  ItemsWidget->SortItems(
      {.SortType = ESortType::Price,
       .bReverse = ItemsWidget->SortData.SortType == ESortType::Price ? !ItemsWidget->SortData.bReverse : false});

  SortByMarketPriceButton->SetActiveStyle(true);
  SortByNameButton->SetActiveStyle(false);

  UGameplayStatics::PlaySound2D(this, SortByMarketPriceButton->ClickedSound, 1.0f);
}
void UInventoryViewWidget::SortByName() {
  ItemsWidget->SortItems(
      {.SortType = ESortType::Name,
       .bReverse = ItemsWidget->SortData.SortType == ESortType::Name ? !ItemsWidget->SortData.bReverse : false});

  SortByMarketPriceButton->SetActiveStyle(false);
  SortByNameButton->SetActiveStyle(true);

  UGameplayStatics::PlaySound2D(this, SortByMarketPriceButton->ClickedSound, 1.0f);
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

void UInventoryViewWidget::InitUI(FInUIInputActions InUIInputActions,
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

  ItemsWidget->InitUI(InventoryC,
                      "Bought At:", [this](FName ItemID) -> float { return MarketEconomy->GetMarketPrice(ItemID); });
  if (ItemsWidget->SortData.SortType == ESortType::None) SortByMarketPrice();

  SortByMarketPriceButton->ActionText->SetText(FText::FromString("Sort - Price"));
  SortByNameButton->ActionText->SetText(FText::FromString("Sort - Name"));

  BackButton->ActionText->SetText(FText::FromString("Back"));

  CloseWidgetFunc = _CloseWidgetFunc;

  SortByMarketPriceButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton1Action);
  SortByNameButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.UISideButton2Action);
  BackButton->CommonActionWidget->SetEnhancedInputAction(InUIInputActions.RetractUIAction);
}

void UInventoryViewWidget::SetupUIActionable() {
  UIActionable.DirectionalInput = [this](FVector2D Direction) { ItemsWidget->SelectNextItem(Direction); };
  UIActionable.SideButton1 = [this]() { SortByMarketPrice(); };
  UIActionable.SideButton2 = [this]() { SortByName(); };
  UIActionable.CycleLeft = [this]() { MenuHeaderWidget->CycleLeft(); };
  UIActionable.CycleRight = [this]() { MenuHeaderWidget->CycleRight(); };
  UIActionable.RetractUI = [this]() { Back(); };
}

void UInventoryViewWidget::SetupUIBehaviour() {
  FWidgetAnimationDynamicEvent UIAnimEvent;
  UIAnimEvent.BindDynamic(this, &UInventoryViewWidget::UIAnimComplete);
  BindToAnimationFinished(ShowAnim, UIAnimEvent);
  BindToAnimationFinished(HideAnim, UIAnimEvent);

  UIBehaviour.ShowUI = [this](std::function<void()> Callback) {
    UUserWidget::StopAllAnimations();
    check(!UUserWidget::IsAnimationPlaying(HideAnim));

    UIAnimCompleteFunc = Callback;
    SetVisibility(ESlateVisibility::Visible);

    PlayAnimation(ShowAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
  };
  UIBehaviour.HideUI = [this](std::function<void()> Callback) {
    UUserWidget::StopAllAnimations();
    check(!UUserWidget::IsAnimationPlaying(ShowAnim));

    UIAnimCompleteFunc = [this, Callback]() {
      SetVisibility(ESlateVisibility::Collapsed);

      if (Callback) Callback();
    };

    UUserWidget::PlayAnimation(HideAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
  };
}
void UInventoryViewWidget::UIAnimComplete() {
  if (UIAnimCompleteFunc) UIAnimCompleteFunc();
}