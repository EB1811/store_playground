#include "SpgHUD.h"
#include "GameFramework/PlayerController.h"
#include "store_playground/UI/MainMenu/MainMenuWidget.h"
#include "store_playground/UI/Inventory/PlayerAndContainerWidget.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Market/Market.h"
#include "store_playground/UI/Inventory/InventoryWidget.h"
#include "store_playground/UI/Inventory/InventoryViewWidget.h"
#include "store_playground/UI/Dialogue/DialogueWidget.h"
#include "store_playground/UI/Negotiation/NegotiationWidget.h"
#include "store_playground/UI/NPCStore/NpcStoreWidget.h"
#include "store_playground/UI/Store/StockDisplayWidget.h"
#include "store_playground/UI/Store/BuildableDisplayWidget.h"
#include "store_playground/UI/Newspaper/NewspaperWidget.h"
#include "store_playground/UI/Ability/AbilityWidget.h"
#include "store_playground/Store/Store.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/UI/Upgrade/UpgradeListWidget.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/Upgrade/UpgradeSelectComponent.h"
#include "store_playground/Minigame/MiniGameComponent.h"
#include "store_playground/Minigame/MiniGameManager.h"
#include "Components/TextBlock.h"

ASpgHUD::ASpgHUD() { HUDState = EHUDState::InGame; }

void ASpgHUD::DrawHUD() { Super::DrawHUD(); }

void ASpgHUD::BeginPlay() {
  Super::BeginPlay();

  check(MainMenuWidgetClass);
  check(InventoryViewWidgetClass);
  check(PlayerAndContainerWidgetClass);
  check(BuildableDisplayWidgetClass);
  check(NpcStoreWidgetClass);
  check(UNegotiationWidgetClass);
  check(UDialogueWidgetClass);
  check(StockDisplayWidgetClass);
  check(NewspaperWidgetClass);
  check(UpgradeSelectWidgetClass);
  check(AbilityWidgetClass);

  MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
  MainMenuWidget->AddToViewport(20);
  MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);

  InventoryViewWidget = CreateWidget<UInventoryViewWidget>(GetWorld(), InventoryViewWidgetClass);
  InventoryViewWidget->AddToViewport(10);
  InventoryViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  BuildableDisplayWidget = CreateWidget<UBuildableDisplayWidget>(GetWorld(), BuildableDisplayWidgetClass);
  BuildableDisplayWidget->AddToViewport(10);
  BuildableDisplayWidget->SetVisibility(ESlateVisibility::Collapsed);

  StockDisplayWidget = CreateWidget<UStockDisplayWidget>(GetWorld(), StockDisplayWidgetClass);
  StockDisplayWidget->AddToViewport(10);
  StockDisplayWidget->SetVisibility(ESlateVisibility::Collapsed);

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

  NewspaperWidget = CreateWidget<UNewspaperWidget>(GetWorld(), NewspaperWidgetClass);
  NewspaperWidget->AddToViewport(10);
  NewspaperWidget->SetVisibility(ESlateVisibility::Collapsed);

  UpgradeSelectWidget = CreateWidget<UUpgradeListWidget>(GetWorld(), UpgradeSelectWidgetClass);
  UpgradeSelectWidget->AddToViewport(10);
  UpgradeSelectWidget->SetVisibility(ESlateVisibility::Collapsed);

  AbilityWidget = CreateWidget<UAbilityWidget>(GetWorld(), AbilityWidgetClass);
  AbilityWidget->AddToViewport(10);
  AbilityWidget->SetVisibility(ESlateVisibility::Collapsed);

  const FInputModeGameOnly InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(false);
}

void ASpgHUD::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ASpgHUD::LeaveHUD() {
  const FInputModeGameOnly InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(false);

  SetPlayerNormalFunc();
}

void ASpgHUD::OpenFocusedMenu(UUserWidget* Widget) {
  for (auto* W : OpenedWidgets) W->SetVisibility(ESlateVisibility::Collapsed);
  OpenedWidgets.Empty();

  Widget->SetVisibility(ESlateVisibility::Visible);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(Widget);

  SetPlayerFocussedFunc();
}

void ASpgHUD::OpenMainMenu() {
  if (OpenedWidgets.Contains(MainMenuWidget)) return CloseWidget(MainMenuWidget);

  MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
  const FInputModeUIOnly InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(MainMenuWidget);
}

// ! System state is preserved (e.g., dialogue, negotiation).
void ASpgHUD::CloseTopOpenMenu() {
  if (OpenedWidgets.IsEmpty()) return;

  OpenedWidgets.Pop()->SetVisibility(ESlateVisibility::Collapsed);

  if (!OpenedWidgets.IsEmpty()) return;
  LeaveHUD();
}

void ASpgHUD::CloseAllMenus() {
  for (UUserWidget* Widget : OpenedWidgets) Widget->SetVisibility(ESlateVisibility::Collapsed);

  OpenedWidgets.Empty();
  LeaveHUD();
}

void ASpgHUD::CloseWidget(UUserWidget* Widget) {
  if (!Widget) return;

  Widget->SetVisibility(ESlateVisibility::Collapsed);
  OpenedWidgets.RemoveSingle(Widget);

  if (!OpenedWidgets.IsEmpty()) return;
  LeaveHUD();
}

void ASpgHUD::AdvanceUI() {
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FProperty* UIActionableProp = TopWidget->GetClass()->FindPropertyByName("UIActionable");
  if (!UIActionableProp) return;

  FUIActionable* ActionableWidget = UIActionableProp->ContainerPtrToValuePtr<FUIActionable>(TopWidget);
  ActionableWidget->AdvanceUI();
}

// todo-low: Update.
void ASpgHUD::SetAndOpenInventoryView(UInventoryComponent* PlayerInventory, AStore* Store) {
  check(InventoryViewWidget);
  if (OpenedWidgets.Contains(InventoryViewWidget)) return CloseWidget(InventoryViewWidget);

  InventoryViewWidget->PlayerInventoryWidget->InventoryRef = PlayerInventory;
  InventoryViewWidget->PlayerInventoryWidget->InventoryTitleText->SetText(FText::FromString("Player"));

  InventoryViewWidget->Store = Store;

  InventoryViewWidget->RefreshInventoryViewUI();

  InventoryViewWidget->SetVisibility(ESlateVisibility::Visible);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(InventoryViewWidget);
}

void ASpgHUD::SetAndOpenBuildableDisplay(ABuildable* Buildable,
                                         std::function<bool(class ABuildable* Buildable)> BuildStockDisplayFunc,
                                         std::function<bool(class ABuildable* Buildable)> BuildDecorationFunc) {
  check(BuildableDisplayWidget);

  BuildableDisplayWidget->SetBuildable(Buildable);
  BuildableDisplayWidget->BuildStockDisplayFunc = BuildStockDisplayFunc;
  BuildableDisplayWidget->BuildDecorationFunc = BuildDecorationFunc;
  BuildableDisplayWidget->CloseWidgetFunc = [this]() { CloseWidget(BuildableDisplayWidget); };

  BuildableDisplayWidget->SetVisibility(ESlateVisibility::Visible);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(BuildableDisplayWidget);
}

void ASpgHUD::SetAndOpenStockDisplay(UStockDisplayComponent* StockDisplay,
                                     UInventoryComponent* DisplayInventory,
                                     UInventoryComponent* PlayerInventory,
                                     std::function<void(UItemBase*, UInventoryComponent*)> PlayerToDisplayFunc,
                                     std::function<void(UItemBase*, UInventoryComponent*)> DisplayToPlayerFunc) {
  check(StockDisplayWidget);

  // StockDisplayWidget->StockDisplayRef = StockDisplay;
  StockDisplayWidget->PlayerAndContainerWidget->PlayerInventoryWidget->InventoryRef = PlayerInventory;
  StockDisplayWidget->PlayerAndContainerWidget->PlayerInventoryWidget->InventoryTitleText->SetText(
      FText::FromString("Player"));
  StockDisplayWidget->PlayerAndContainerWidget->ContainerInventoryWidget->InventoryRef = DisplayInventory;
  StockDisplayWidget->PlayerAndContainerWidget->ContainerInventoryWidget->InventoryTitleText->SetText(
      FText::FromString("Display"));

  StockDisplayWidget->PlayerAndContainerWidget->PlayerInventoryWidget->RefreshInventory();
  StockDisplayWidget->PlayerAndContainerWidget->ContainerInventoryWidget->RefreshInventory();

  StockDisplayWidget->PlayerAndContainerWidget->PlayerInventoryWidget->OnDropItemFunc =
      [DisplayToPlayerFunc, DisplayInventory](UItemBase* Item, int32 Quantity) {
        DisplayToPlayerFunc(Item, DisplayInventory);
      };
  StockDisplayWidget->PlayerAndContainerWidget->ContainerInventoryWidget->OnDropItemFunc =
      [PlayerToDisplayFunc, PlayerInventory](UItemBase* Item, int32 Quantity) {
        PlayerToDisplayFunc(Item, PlayerInventory);
      };

  StockDisplayWidget->SetVisibility(ESlateVisibility::Visible);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(StockDisplayWidget);
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

// todo-low: Check for refresh of open widgets.
void ASpgHUD::SetAndOpenNPCStore(UInventoryComponent* NPCStoreInventory,
                                 UInventoryComponent* PlayerInventory,
                                 std::function<void(class UItemBase*, class UInventoryComponent*)> PlayerToStoreFunc,
                                 std::function<void(class UItemBase*, class UInventoryComponent*)> StoreToPlayerFunc) {
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
      [this, StoreToPlayerFunc, NPCStoreInventory](UItemBase* Item, int32 Quantity) {
        StoreToPlayerFunc(Item, NPCStoreInventory);

        if (InventoryViewWidget->IsVisible()) InventoryViewWidget->RefreshInventoryViewUI();
      };
  NpcStoreWidget->PlayerAndContainerWidget->ContainerInventoryWidget->OnDropItemFunc =
      [this, PlayerToStoreFunc, PlayerInventory](UItemBase* Item, int32 Quantity) {
        PlayerToStoreFunc(Item, PlayerInventory);

        if (InventoryViewWidget->IsVisible()) InventoryViewWidget->RefreshInventoryViewUI();
      };

  OpenFocusedMenu(NpcStoreWidget);
}

void ASpgHUD::SetAndOpenDialogue(UDialogueSystem* Dialogue, std::function<void()> OnDialogueEndFunc) {
  check(UDialogueWidgetClass);

  DialogueWidget->CloseDialogueUI = [this, OnDialogueEndFunc] {
    CloseWidget(DialogueWidget);

    if (OnDialogueEndFunc) OnDialogueEndFunc();
  };
  DialogueWidget->InitDialogueUI(Dialogue);

  OpenFocusedMenu(DialogueWidget);
}

void ASpgHUD::SetAndOpenNegotiation(const UNegotiationSystem* Negotiation, UInventoryComponent* PlayerInventoryC) {
  check(UNegotiationWidgetClass);

  NegotiationWidget->NegotiationSystemRef = const_cast<UNegotiationSystem*>(Negotiation);
  NegotiationWidget->PlayerInventoryRef = PlayerInventoryC;
  NegotiationWidget->CloseNegotiationUI = [this] { CloseWidget(NegotiationWidget); };
  NegotiationWidget->RefreshInventoryUI = [this] {
    if (InventoryViewWidget->IsVisible()) InventoryViewWidget->RefreshInventoryViewUI();
  };
  NegotiationWidget->InitNegotiationUI();
  NegotiationWidget->SetVisibility(ESlateVisibility::Visible);

  OpenFocusedMenu(NegotiationWidget);
}

void ASpgHUD::SetAndOpenNewspaper(const ANewsGen* NewsGenRef) {
  check(NewspaperWidget);

  if (OpenedWidgets.Contains(NewspaperWidget)) return CloseWidget(NewspaperWidget);

  NewspaperWidget->NewsGenRef = NewsGenRef;
  NewspaperWidget->RefreshNewspaperUI();

  NewspaperWidget->SetVisibility(ESlateVisibility::Visible);

  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(NewspaperWidget);
}

void ASpgHUD::SetAndOpenUpgradeSelect(UUpgradeSelectComponent* UpgradeSelectC, AUpgradeManager* UpgradeManager) {
  check(UpgradeSelectWidget);

  UpgradeSelectWidget->UpgradeManagerRef = UpgradeManager;

  UpgradeSelectWidget->SelectUpgradeFunc = [this, UpgradeManager](FName UpgradeID) {
    UpgradeManager->SelectUpgrade(UpgradeID);
  };
  UpgradeSelectWidget->SetUpgradeClass(UpgradeSelectC->UpgradeClass, UpgradeSelectC->Title,
                                       UpgradeSelectC->Description);

  OpenFocusedMenu(UpgradeSelectWidget);
}

void ASpgHUD::SetAndOpenAbilitySelect(class AAbilityManager* AbilityManager) {
  check(AbilityWidget);

  AbilityWidget->AbilityManagerRef = AbilityManager;
  AbilityWidget->RefreshUI();

  OpenFocusedMenu(AbilityWidget);
}

void ASpgHUD::SetAndOpenMiniGame(AMiniGameManager* MiniGameManager,
                                 UMiniGameComponent* MiniGameC,
                                 AStore* Store,
                                 UInventoryComponent* PlayerInventory) {
  UUserWidget* MiniGameWidget = MiniGameManager->GetMiniGameWidget(
      MiniGameC->MiniGameType, Store, PlayerInventory, [this](UUserWidget* Widget) { CloseWidget(Widget); });

  MiniGameWidget->AddToViewport(10);
  OpenFocusedMenu(MiniGameWidget);
}

void ASpgHUD::SetAndOpenDialogueCutscene(UDialogueSystem* Dialogue) {
  check(UDialogueWidgetClass);

  DialogueWidget->CloseDialogueUI = [this] { CloseWidget(DialogueWidget); };
  DialogueWidget->InitDialogueUI(Dialogue);

  OpenFocusedMenu(DialogueWidget);
  SetPlayerCutsceneFunc();
}