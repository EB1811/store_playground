#include "CompactItemDetailsWidget.h"
#include "store_playground/Item/ItemBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCompactItemDetailsWidget::InitUI(const UItemBase* Item,
                                       FName _ShowPriceText,
                                       float _MarketPrice,
                                       float _ShowPrice) {
  check(Item);

  Name->SetText(Item->TextData.Name);
  EconInfo->SetText(
      FText::FromString(FString::Printf(TEXT("%s, %s"), *(UEnum::GetDisplayValueAsText(Item->ItemType)).ToString(),
                                        *(UEnum::GetDisplayValueAsText(Item->ItemWealthType).ToString()))));
  Icon->SetBrushFromTexture(Item->AssetData.Icon);

  if (Item->Quantity > 1) {
    Quantity->SetText(FText::FromString("X" + FString::FromInt(Item->Quantity)));
    Quantity->SetVisibility(ESlateVisibility::Visible);
  } else {
    Quantity->SetVisibility(ESlateVisibility::Collapsed);
  }

  MarketPrice->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), _MarketPrice)));

  if (_ShowPrice > 0.0f) {
    ShowPriceText->SetText(FText::FromName(_ShowPriceText));
    ShowPrice->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), _ShowPrice)));
    ShowPriceText->SetVisibility(ESlateVisibility::Visible);
    ShowPrice->SetVisibility(ESlateVisibility::Visible);
  } else {
    ShowPriceText->SetVisibility(ESlateVisibility::Collapsed);
    ShowPrice->SetVisibility(ESlateVisibility::Collapsed);
  }
}