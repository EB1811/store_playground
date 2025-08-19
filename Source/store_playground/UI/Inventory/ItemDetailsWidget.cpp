#include "ItemDetailsWidget.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/UI/Graph/PriceGraphWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemDetailsWidget::RefreshUI() {
  check(ItemRef && MarketPriceFunc);

  Name->SetText(ItemRef->TextData.Name);
  EconInfo->SetText(
      FText::FromString(FString::Printf(TEXT("%s, %s"), *(UEnum::GetDisplayValueAsText(ItemRef->ItemType)).ToString(),
                                        *(UEnum::GetDisplayValueAsText(ItemRef->ItemWealthType).ToString()))));
  Desc->SetText(ItemRef->TextData.Description);
  Icon->SetBrushFromTexture(ItemRef->AssetData.Icon);

  if (ItemRef->Quantity > 1) {
    Quantity->SetText(FText::FromString("X" + FString::FromInt(ItemRef->Quantity)));
    Quantity->SetVisibility(ESlateVisibility::Visible);
  } else {
    Quantity->SetVisibility(ESlateVisibility::Collapsed);
  }

  float _ShowPrice = ShowPriceFunc ? ShowPriceFunc(ItemRef->ItemID) : ItemRef->PlayerPriceData.BoughtAt;
  ShowPrice->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), _ShowPrice)));
  float _MarketPrice = MarketPriceFunc(ItemRef->ItemID);
  MarketPrice->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), _MarketPrice)));

  PriceGraphWidget->RefreshUI();
}

void UItemDetailsWidget::InitUI(const UItemBase* _ItemRef,
                                FName _ShowPriceText,
                                std::function<float(FName)> _MarketPriceFunc,
                                const TArray<float>* PriceHistory,
                                std::function<float(FName)> _ShowPriceFunc) {
  check(_ItemRef && _MarketPriceFunc);

  ItemRef = _ItemRef;
  ShowPriceText->SetText(FText::FromName(_ShowPriceText));
  MarketPriceFunc = _MarketPriceFunc;
  ShowPriceFunc = _ShowPriceFunc;

  PriceGraphWidget->InitUI(PriceHistory, ItemRef->PlayerPriceData.BoughtAt);
}