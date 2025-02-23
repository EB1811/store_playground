#include "SpgHUD.h"
#include "store_playground/UI/Inventory/PlayerAndContainerWidget.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/UI/Inventory/InventoryWidget.h"
#include "store_playground/UI/Inventory/PlayerInventoryWidget.h"
#include "store_playground/UI/Dialogue/DialogueWidget.h"
#include "store_playground/UI/Negotiation/NegotiationWidget.h"

ASpgHUD::ASpgHUD() {}

void ASpgHUD::DrawHUD() { Super::DrawHUD(); }

void ASpgHUD::BeginPlay() {
  Super::BeginPlay();

  check(PlayerAndContainerWidgetClass);
  check(PlayerInventoryWidgetClass);
  check(UNegotiationWidgetClass);
  check(UDialogueWidgetClass);

  PlayerInventoryWidget = CreateWidget<UPlayerInventoryWidget>(GetWorld(), PlayerInventoryWidgetClass);
  PlayerInventoryWidget->AddToViewport(10);
  PlayerInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);

  PlayerAndContainerWidget = CreateWidget<UPlayerAndContainerWidget>(GetWorld(), PlayerAndContainerWidgetClass);
  PlayerAndContainerWidget->AddToViewport(10);
  PlayerAndContainerWidget->SetVisibility(ESlateVisibility::Collapsed);

  DialogueWidget = CreateWidget<UDialogueWidget>(GetWorld(), UDialogueWidgetClass);
  DialogueWidget->AddToViewport(10);
  DialogueWidget->SetVisibility(ESlateVisibility::Collapsed);

  NegotiationWidget = CreateWidget<UNegotiationWidget>(GetWorld(), UNegotiationWidgetClass);
  NegotiationWidget->AddToViewport(10);
  NegotiationWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void ASpgHUD::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ASpgHUD::CloseTopOpenMenu() {
  if (OpenedWidgets.IsEmpty()) return;

  OpenedWidgets.Pop()->SetVisibility(ESlateVisibility::Collapsed);

  if (!OpenedWidgets.IsEmpty()) return;
  const FInputModeGameOnly InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(false);
}

void ASpgHUD::CloseAllMenus() {
  for (UUserWidget* Widget : OpenedWidgets) Widget->SetVisibility(ESlateVisibility::Collapsed);

  OpenedWidgets.Empty();

  const FInputModeGameOnly InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(false);
}

void ASpgHUD::CloseWidget(UUserWidget* Widget) {
  if (!Widget) return;

  Widget->SetVisibility(ESlateVisibility::Collapsed);
  OpenedWidgets.RemoveSingle(Widget);

  if (!OpenedWidgets.IsEmpty()) return;

  const FInputModeGameOnly InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(false);
}

void ASpgHUD::SetAndOpenInventory(const UInventoryComponent* Inventory) {
  check(PlayerInventoryWidget);
  if (OpenedWidgets.Contains(PlayerInventoryWidget)) return CloseWidget(PlayerInventoryWidget);

  PlayerInventoryWidget->InventoryWidget->InventoryRef = const_cast<UInventoryComponent*>(Inventory);
  PlayerInventoryWidget->InventoryWidget->RefreshInventory();
  PlayerInventoryWidget->SetVisibility(ESlateVisibility::Visible);

  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(PlayerInventoryWidget);
}

void ASpgHUD::SetAndOpenContainer(const UInventoryComponent* PlayerInventory,
                                  const UInventoryComponent* ContainerInventory) {
  check(PlayerAndContainerWidget);

  PlayerAndContainerWidget->PlayerInventoryWidget->InventoryRef = const_cast<UInventoryComponent*>(PlayerInventory);
  PlayerAndContainerWidget->PlayerInventoryWidget->RefreshInventory();
  PlayerAndContainerWidget->ContainerInventoryWidget->InventoryRef =
      const_cast<UInventoryComponent*>(ContainerInventory);
  PlayerAndContainerWidget->ContainerInventoryWidget->RefreshInventory();

  PlayerAndContainerWidget->SetVisibility(ESlateVisibility::Visible);

  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(PlayerAndContainerWidget);
}

void ASpgHUD::SetAndOpenDialogue(UDialogueSystem* Dialogue) {
  check(UDialogueWidgetClass);

  DialogueWidget->CloseDialogueUI = [this] { CloseWidget(DialogueWidget); };
  DialogueWidget->InitDialogueUI(Dialogue);
  DialogueWidget->SetVisibility(ESlateVisibility::Visible);

  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(DialogueWidget);
}

void ASpgHUD::SetAndOpenNegotiation(const UNegotiationSystem* Negotiation) {
  check(UNegotiationWidgetClass);

  NegotiationWidget->NegotiationSystemRef = const_cast<UNegotiationSystem*>(Negotiation);
  NegotiationWidget->CloseNegotiationUI = [this] { CloseWidget(NegotiationWidget); };
  NegotiationWidget->RefreshInventoryUI = [this] {
    if (PlayerInventoryWidget->IsVisible()) PlayerInventoryWidget->InventoryWidget->RefreshInventory();
  };
  NegotiationWidget->InitNegotiationUI();
  NegotiationWidget->SetVisibility(ESlateVisibility::Visible);

  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(NegotiationWidget);
}