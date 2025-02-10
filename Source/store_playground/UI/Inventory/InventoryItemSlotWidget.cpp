#include "InventoryItemSlotWidget.h"
#include "store_playground/Item/ItemBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventoryItemSlotWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

void UInventoryItemSlotWidget::NativeConstruct() {
  Super::NativeConstruct();

  if (!ItemRef) return;

  ItemIcon->SetBrushFromTexture(ItemRef->AssetData.Icon);
  ItemName->SetText(ItemRef->FlavorData.TextData.Name);
}
