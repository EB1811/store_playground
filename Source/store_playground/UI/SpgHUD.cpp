#include "SpgHUD.h"
#include "store_playground/UI/Inventory/InventoryWidget.h"
#include "store_playground/Inventory/InventoryComponent.h"

ASpgHUD::ASpgHUD() {}

void ASpgHUD::DrawHUD() { Super::DrawHUD(); }

void ASpgHUD::BeginPlay() {
  Super::BeginPlay();

  if (ContainerWidgetClass) {
    ItemContainerWidget = CreateWidget<UInventoryWidget>(GetWorld(), ContainerWidgetClass);
    ItemContainerWidget->AddToViewport(10);
    ItemContainerWidget->SetVisibility(ESlateVisibility::Collapsed);
  }
}

void ASpgHUD::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ASpgHUD::SetAndOpenContainer(const UInventoryComponent* InventoryComponent) {
  if (ItemContainerWidget) {
    ItemContainerWidget->InventoryRef = const_cast<UInventoryComponent*>(InventoryComponent);
    ItemContainerWidget->RefreshInventory();

    ItemContainerWidget->SetVisibility(ESlateVisibility::Visible);
  }
}

void ASpgHUD::CloseContainer() {
  if (ItemContainerWidget) ItemContainerWidget->SetVisibility(ESlateVisibility::Collapsed);
}