#include "SpgHUD.h"
#include "store_playground/UI/Inventory/PlayerAndContainerWidget.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Market/Market.h"
#include "store_playground/UI/Inventory/InventoryWidget.h"
#include "store_playground/UI/Inventory/InventoryViewWidget.h"
#include "store_playground/UI/Dialogue/DialogueWidget.h"
#include "store_playground/UI/Negotiation/NegotiationWidget.h"
#include "store_playground/UI/NPCStore/NpcStoreWidget.h"
#include "store_playground/Store/Store.h"
#include "Components/TextBlock.h"

ASpgHUD::ASpgHUD() {}

void ASpgHUD::DrawHUD() { Super::DrawHUD(); }

void ASpgHUD::BeginPlay() {
  Super::BeginPlay();

  check(InventoryViewWidgetClass);
  check(PlayerAndContainerWidgetClass);
  check(NpcStoreWidgetClass);
  check(UNegotiationWidgetClass);
  check(UDialogueWidgetClass);

  InventoryViewWidget = CreateWidget<UInventoryViewWidget>(GetWorld(), InventoryViewWidgetClass);
  InventoryViewWidget->AddToViewport(10);
  InventoryViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  PlayerAndContainerWidget = CreateWidget<UPlayerAndContainerWidget>(GetWorld(), PlayerAndContainerWidgetClass);
  PlayerAndContainerWidget->AddToViewport(10);
  PlayerAndContainerWidget->SetVisibility(ESlateVisibility::Collapsed);

  NpcStoreWidget = CreateWidget<UNpcStoreWidget>(GetWorld(), NpcStoreWidgetClass);
  NpcStoreWidget->AddToViewport(10);
  NpcStoreWidget->SetVisibility(ESlateVisibility::Collapsed);

  DialogueWidget = CreateWidget<UDialogueWidget>(GetWorld(), UDialogueWidgetClass);
  DialogueWidget->AddToViewport(10);
  DialogueWidget->SetVisibility(ESlateVisibility::Collapsed);

  NegotiationWidget = CreateWidget<UNegotiationWidget>(GetWorld(), UNegotiationWidgetClass);
  NegotiationWidget->AddToViewport(10);
  NegotiationWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void ASpgHUD::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

// ! System state is preserved (e.g., dialogue, negotiation).
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

void ASpgHUD::SetAndOpenInventoryView(UInventoryComponent* PlayerInventory, AStore* Store) {
  check(InventoryViewWidget);
  if (OpenedWidgets.Contains(InventoryViewWidget)) return CloseWidget(InventoryViewWidget);

  InventoryViewWidget->PlayerInventoryWidget->InventoryRef = PlayerInventory;
  InventoryViewWidget->PlayerInventoryWidget->InventoryTitleText->SetText(FText::FromString("Player"));
  InventoryViewWidget->StoreInventoryWidget->InventoryRef = Store->StoreStock;
  InventoryViewWidget->StoreInventoryWidget->InventoryTitleText->SetText(FText::FromString("Store"));

  InventoryViewWidget->Store = Store;

  InventoryViewWidget->RefreshInventoryViewUI();

  InventoryViewWidget->SetVisibility(ESlateVisibility::Visible);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(InventoryViewWidget);
}

void ASpgHUD::SetAndOpenContainer(const UInventoryComponent* PlayerInventory,
                                  const UInventoryComponent* ContainerInventory) {
  check(PlayerAndContainerWidget);

  PlayerAndContainerWidget->PlayerInventoryWidget->InventoryRef = const_cast<UInventoryComponent*>(PlayerInventory);
  PlayerAndContainerWidget->PlayerInventoryWidget->InventoryTitleText->SetText(FText::FromString("Player"));
  PlayerAndContainerWidget->ContainerInventoryWidget->InventoryRef =
      const_cast<UInventoryComponent*>(ContainerInventory);
  PlayerAndContainerWidget->ContainerInventoryWidget->InventoryTitleText->SetText(FText::FromString("Container"));

  PlayerAndContainerWidget->PlayerInventoryWidget->RefreshInventory();
  PlayerAndContainerWidget->ContainerInventoryWidget->RefreshInventory();

  PlayerAndContainerWidget->SetVisibility(ESlateVisibility::Visible);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(PlayerAndContainerWidget);
}

// TODO: Check for refresh of open widgets.
void ASpgHUD::SetAndOpenNPCStore(UInventoryComponent* NPCStoreInventory,
                                 UInventoryComponent* PlayerInventory,
                                 AStore* PlayerStore) {
  check(NpcStoreWidget);

  NpcStoreWidget->PlayerAndContainerWidget->PlayerInventoryWidget->InventoryRef = PlayerInventory;
  NpcStoreWidget->PlayerAndContainerWidget->PlayerInventoryWidget->InventoryTitleText->SetText(
      FText::FromString("Player"));
  NpcStoreWidget->PlayerAndContainerWidget->ContainerInventoryWidget->InventoryRef = NPCStoreInventory;
  NpcStoreWidget->PlayerAndContainerWidget->ContainerInventoryWidget->InventoryTitleText->SetText(
      FText::FromString("NPC Store"));

  NpcStoreWidget->PlayerAndContainerWidget->PlayerInventoryWidget->RefreshInventory();
  NpcStoreWidget->PlayerAndContainerWidget->ContainerInventoryWidget->RefreshInventory();

  NpcStoreWidget->PlayerAndContainerWidget->PlayerInventoryWidget->OnDropItemFunc =
      [this, NPCStoreInventory, PlayerInventory, PlayerStore](UItemBase* Item, int32 Quantity) {
        BuyItem(NPCStoreInventory, PlayerInventory, PlayerStore, Item, Quantity);
        if (InventoryViewWidget->IsVisible()) InventoryViewWidget->RefreshInventoryViewUI();
      };
  NpcStoreWidget->PlayerAndContainerWidget->ContainerInventoryWidget->OnDropItemFunc =
      [this, PlayerInventory, NPCStoreInventory, PlayerStore](UItemBase* Item, int32 Quantity) {
        SellItem(NPCStoreInventory, PlayerInventory, PlayerStore, Item, Quantity);
        if (InventoryViewWidget->IsVisible()) InventoryViewWidget->RefreshInventoryViewUI();
      };

  NpcStoreWidget->SetVisibility(ESlateVisibility::Visible);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(NpcStoreWidget);
}

void ASpgHUD::SetAndOpenDialogue(UDialogueSystem* Dialogue, std::function<void()> OnDialogueEndFunc) {
  check(UDialogueWidgetClass);

  DialogueWidget->CloseDialogueUI = [this, OnDialogueEndFunc] {
    CloseWidget(DialogueWidget);

    if (OnDialogueEndFunc) OnDialogueEndFunc();
  };
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
    if (InventoryViewWidget->IsVisible()) InventoryViewWidget->RefreshInventoryViewUI();
  };
  NegotiationWidget->InitNegotiationUI();
  NegotiationWidget->SetVisibility(ESlateVisibility::Visible);

  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(NegotiationWidget);
}