#include "InventoryItemSlotWidget.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/UI/Inventory/InventoryWidget.h"
#include "store_playground/UI/Inventory/ItemDragDropOp.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventoryItemSlotWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

void UInventoryItemSlotWidget::NativeConstruct() {
  Super::NativeConstruct();

  if (!ItemRef) return;

  ItemIcon->SetBrushFromTexture(ItemRef->AssetData.Icon);
  ItemName->SetText(ItemRef->FlavorData.TextData.Name);
  if (ItemRef->Quantity > 1) ItemQuantity->SetText(FText::FromString(FString::FromInt(ItemRef->Quantity)));
}

FReply UInventoryItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,
                                                         const FPointerEvent& InMouseEvent) {
  FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

  if (InMouseEvent.GetEffectingButton() == (EKeys::LeftMouseButton))
    return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);

  return Reply.Unhandled();
}

void UInventoryItemSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {
  Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventoryItemSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry,
                                                    const FPointerEvent& InMouseEvent,
                                                    UDragDropOperation*& OutOperation) {
  Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
  check(ItemRef && InventoryWidgetRef->InventoryRef);

  // TObjectPtr<UDragItemVisual> DragVisual = CreateWidget<UDragItemVisual>(this, DragVisualClass);
  // DragVisual->ItemIcon->SetBrushFromTexture(ItemRef->AssetData.Icon);
  // DragVisual->ItemName->SetText(FText::FromString(ItemRef->TextData.Name.ToString()));

  UItemDragDropOp* ItemDragOperation = NewObject<UItemDragDropOp>();
  ItemDragOperation->SourceItem = ItemRef;
  ItemDragOperation->SourceInventory = InventoryWidgetRef->InventoryRef;
  ItemDragOperation->SourceInventoryWidget = InventoryWidgetRef;
  // ItemDragOperation->DefaultDragVisual = DragVisual;
  ItemDragOperation->Pivot = EDragPivot::MouseDown;

  OutOperation = ItemDragOperation;
}

bool UInventoryItemSlotWidget::NativeOnDrop(const FGeometry& InGeometry,
                                            const FDragDropEvent& InDragDropEvent,
                                            UDragDropOperation* InOperation) {
  return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}