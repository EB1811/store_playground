#include "ItemSlotWidget.h"
#include "Math/Color.h"
#include "store_playground/Item/ItemBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UItemSlotWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  SelectItemButton->OnClicked.AddDynamic(this, &UItemSlotWidget::OnSelectItemButtonClicked);
}

void UItemSlotWidget::RefreshUI() {
  check(ItemRef);

  Icon->SetBrushFromTexture(ItemRef->AssetData.Icon);
  if (ItemRef->TextData.Name.ToString().Len() <= 12)
    Name->SetText(FText::FromString(ItemRef->TextData.Name.ToString()));
  else Name->SetText(FText::FromString(ItemRef->TextData.Name.ToString().Left(9) + "..."));

  if (ItemRef->Quantity > 1) Quantity->SetText(FText::FromString(FString::FromInt(ItemRef->Quantity)));
  else Quantity->SetText(FText::GetEmpty());

  // float Price = ShowPriceFunc ? ShowPriceFunc(ItemRef->ItemID) : ItemRef->PlayerPriceData.BoughtAt;
  check(ShowPriceFunc);
  float Price = ShowPriceFunc(ItemRef->ItemID);
  ShowPrice->SetText(FText::FromString(FString::Printf(TEXT("%.0f¬"), Price)));

  if (bIsSelected) SelectItemButton->SetIsEnabled(false);
  else SelectItemButton->SetIsEnabled(true);
}

void UItemSlotWidget::OnSelectItemButtonClicked() {
  check(OnSelectItemFunc);

  OnSelectItemFunc(ItemRef);
}