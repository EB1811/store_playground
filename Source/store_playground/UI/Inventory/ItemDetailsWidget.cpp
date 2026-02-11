#include "ItemDetailsWidget.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/UI/Graph/PriceGraphWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemDetailsWidget::RefreshUI() {
  check(ItemRef && MarketPriceFunc);

  Name->SetText(ItemRef->TextData.Name);
  EconInfo->SetText(
      FText::FromString(FString::Printf(TEXT("%s, %s"), *(GetItemEconTypeText(ItemRef->ItemEconType).ToString()),
                                        *(GetItemWealthTypeText(ItemRef->ItemWealthType).ToString()))));
  Desc->SetText(ItemRef->TextData.Description);
  Icon->SetBrushFromTexture(ItemRef->AssetData.Icon);

  if (ItemRef->Quantity > 1) {
    Quantity->SetText(FText::FromString("X" + FString::FromInt(ItemRef->Quantity)));
    Quantity->SetVisibility(ESlateVisibility::Visible);
  } else {
    Quantity->SetVisibility(ESlateVisibility::Collapsed);
  }

  float _ShowPrice = ShowPriceFunc ? ShowPriceFunc(ItemRef->ItemID) : ItemRef->PlayerPriceData.BoughtAt;
  float _MarketPrice = MarketPriceFunc(ItemRef->ItemID);
  ShowPrice->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), _ShowPrice)));
  MarketPrice->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), _MarketPrice)));

  if (bShowPriceMargin) {
    float MarginDiff = _ShowPrice - _MarketPrice;
    Margin->SetText(
        FText::FromString(FString::Printf(TEXT("(%.0f"), FMath::Abs(MarginDiff) / _MarketPrice * 100.0f) + TEXT("%)")));

    Margin->SetVisibility(ESlateVisibility::Visible);
  } else {
    Margin->SetVisibility(ESlateVisibility::Collapsed);
  }

  PriceGraphWidget->RefreshUI();
}

void UItemDetailsWidget::InitUI(const UItemBase* _ItemRef,
                                FName _ShowPriceText,
                                std::function<float(FName)> _MarketPriceFunc,
                                const TArray<float>* PriceHistory,
                                std::function<float(FName)> _ShowPriceFunc,
                                bool _bShowPriceMargin) {
  check(_ItemRef && _MarketPriceFunc);

  ItemRef = _ItemRef;
  ShowPriceText->SetText(FText::FromName(_ShowPriceText));
  MarketPriceFunc = _MarketPriceFunc;
  ShowPriceFunc = _ShowPriceFunc;
  bShowPriceMargin = _bShowPriceMargin;

  PriceGraphWidget->InitUI(PriceHistory, ItemRef->PlayerPriceData.BoughtAt);
}