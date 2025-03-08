#include "InventoryViewWidget.h"
#include "Math/UnrealMathUtility.h"
#include "store_playground/Store/Store.h"
#include "InventoryWidget.h"
#include "Components/TextBlock.h"

void UInventoryViewWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

void UInventoryViewWidget::NativeConstruct() { Super::NativeConstruct(); }

void UInventoryViewWidget::RefreshInventoryViewUI() {
  StoreMoneyText->SetText(
      FText::FromString(FString::SanitizeFloat((FMath::RoundToInt32(Store->Money * 100) / 100.00))));

  PlayerInventoryWidget->RefreshInventory();
}