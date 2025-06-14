#include "SpgHUD.h"
#include "GameFramework/PlayerController.h"
#include "store_playground/UI/PauseMenu/PauseMenuWidget.h"
#include "store_playground/UI/Inventory/PlayerAndContainerWidget.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Market/Market.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/UI/Inventory/InventoryWidget.h"
#include "store_playground/UI/Inventory/InventoryViewWidget.h"
#include "store_playground/UI/Dialogue/DialogueWidget.h"
#include "store_playground/UI/Negotiation/NegotiationWidget.h"
#include "store_playground/UI/NPCStore/NpcStoreViewWidget.h"
#include "store_playground/UI/Store/StockDisplayViewWidget.h"
#include "store_playground/UI/Store/BuildableDisplayViewWidget.h"
#include "store_playground/UI/Newspaper/NewsAndEconomyViewWidget.h"
#include "store_playground/UI/Statistics/StatisticsWidget.h"
#include "store_playground/UI/Ability/AbilityViewWidget.h"
#include "store_playground/UI/InGameHud/InGameHudWidget.h"
#include "store_playground/Store/Store.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/UI/Upgrade/UpgradeViewWidget.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/Upgrade/UpgradeSelectComponent.h"
#include "store_playground/Minigame/MiniGameComponent.h"
#include "store_playground/Minigame/MiniGameManager.h"
#include "store_playground/UI/Store/StoreExpansion/StoreExpansionsListWidget.h"
#include "store_playground/UI/Transitions/StorePhaseTransitionWidget.h"
#include "store_playground/UI/Transitions/LevelLoadingTransitionWidget.h"
#include "store_playground/UI/Cutscene/CutsceneWidget.h"
#include "store_playground/UI/WorldUI/Player/InteractionDisplayWidget.h"
#include "store_playground/UI/NpcStore/NpcStoreViewWidget.h"
#include "store_playground/UI/Store/StoreViewWidget.h"
#include "store_playground/Cutscene/CutsceneSystem.h"
#include "Components/TextBlock.h"

ASpgHUD::ASpgHUD() { HUDState = EHUDState::InGame; }

void ASpgHUD::DrawHUD() { Super::DrawHUD(); }

void ASpgHUD::BeginPlay() {
  Super::BeginPlay();

  check(StorePhaseTransitionWidgetClass);
  check(LevelLoadingTransitionWidgetClass);

  StorePhaseTransitionWidget = CreateWidget<UStorePhaseTransitionWidget>(GetWorld(), StorePhaseTransitionWidgetClass);
  LevelLoadingTransitionWidget =
      CreateWidget<ULevelLoadingTransitionWidget>(GetWorld(), LevelLoadingTransitionWidgetClass);

  check(InGameHudWidgetClass);
  check(PauseMenuWidgetClass);
  check(InteractionDisplayWidgetClass);
  check(InventoryViewWidgetClass);
  check(PlayerAndContainerWidgetClass);
  check(BuildableDisplayViewWidgetClass);
  check(NpcStoreViewWidgetClass);
  check(UNegotiationWidgetClass);
  check(UDialogueWidgetClass);
  check(StockDisplayViewWidgetClass);
  check(StoreExpansionsListWidgetClass);
  check(NewsAndEconomyViewWidgetClass);
  check(UpgradeViewWidgetClass);
  check(AbilityViewWidgetClass);

  InGameHudWidget = CreateWidget<UInGameHudWidget>(GetWorld(), InGameHudWidgetClass);
  InGameHudWidget->AddToViewport(0);
  InGameHudWidget->SetVisibility(ESlateVisibility::Collapsed);
  InGameHudWidget->NewsGen = NewsGen;
  InGameHudWidget->DayManager = DayManager;
  InGameHudWidget->StorePhaseManager = StorePhaseManager;
  InGameHudWidget->Store = Store;
  InGameHudWidget->LevelManager = LevelManager;

  PauseMenuWidget = CreateWidget<UPauseMenuWidget>(GetWorld(), PauseMenuWidgetClass);
  PauseMenuWidget->AddToViewport(20);
  PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);

  InteractionPopupWidget = CreateWidget<UInteractionDisplayWidget>(GetWorld(), InteractionDisplayWidgetClass);
  InteractionPopupWidget->AddToViewport(10);
  InteractionPopupWidget->SetVisibility(ESlateVisibility::Collapsed);

  InventoryViewWidget = CreateWidget<UInventoryViewWidget>(GetWorld(), InventoryViewWidgetClass);
  InventoryViewWidget->AddToViewport(10);
  InventoryViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  BuildableDisplayViewWidget = CreateWidget<UBuildableDisplayViewWidget>(GetWorld(), BuildableDisplayViewWidgetClass);
  BuildableDisplayViewWidget->AddToViewport(10);
  BuildableDisplayViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  StockDisplayViewWidget = CreateWidget<UStockDisplayViewWidget>(GetWorld(), StockDisplayViewWidgetClass);
  StockDisplayViewWidget->AddToViewport(10);
  StockDisplayViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  StoreExpansionsListWidget = CreateWidget<UStoreExpansionsListWidget>(GetWorld(), StoreExpansionsListWidgetClass);
  StoreExpansionsListWidget->AddToViewport(10);
  StoreExpansionsListWidget->SetVisibility(ESlateVisibility::Collapsed);

  PlayerAndContainerWidget = CreateWidget<UPlayerAndContainerWidget>(GetWorld(), PlayerAndContainerWidgetClass);
  PlayerAndContainerWidget->AddToViewport(10);
  PlayerAndContainerWidget->SetVisibility(ESlateVisibility::Collapsed);

  NpcStoreViewWidget = CreateWidget<UNpcStoreViewWidget>(GetWorld(), NpcStoreViewWidgetClass);
  NpcStoreViewWidget->AddToViewport(10);
  NpcStoreViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  DialogueWidget = CreateWidget<UDialogueWidget>(GetWorld(), UDialogueWidgetClass);
  DialogueWidget->InitUI(PlayerInputActions);
  DialogueWidget->AddToViewport(10);
  DialogueWidget->SetVisibility(ESlateVisibility::Collapsed);

  NegotiationWidget = CreateWidget<UNegotiationWidget>(GetWorld(), UNegotiationWidgetClass);
  NegotiationWidget->AddToViewport(10);
  NegotiationWidget->SetVisibility(ESlateVisibility::Collapsed);

  CutsceneWidget = CreateWidget<UCutsceneWidget>(GetWorld(), CutsceneWidgetClass);
  CutsceneWidget->AddToViewport(10);
  CutsceneWidget->SetVisibility(ESlateVisibility::Collapsed);

  NewsAndEconomyViewWidget = CreateWidget<UNewsAndEconomyViewWidget>(GetWorld(), NewsAndEconomyViewWidgetClass);
  NewsAndEconomyViewWidget->AddToViewport(10);
  NewsAndEconomyViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  StoreViewWidget = CreateWidget<UStoreViewWidget>(GetWorld(), StoreViewWidgetClass);
  StoreViewWidget->AddToViewport(10);
  StoreViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  UpgradeViewWidget = CreateWidget<UUpgradeViewWidget>(GetWorld(), UpgradeViewWidgetClass);
  UpgradeViewWidget->AddToViewport(10);
  UpgradeViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  AbilityViewWidget = CreateWidget<UAbilityViewWidget>(GetWorld(), AbilityViewWidgetClass);
  AbilityViewWidget->AddToViewport(10);
  AbilityViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  const FInputModeGameOnly InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(false);
}

void ASpgHUD::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ASpgHUD::SetupInitUIStates() {
  // * Input actions are created in SetupPlayerInputComponent, but it runs after BeginPlay, so we need to set them here.
  InGameHudWidget->InitUI(PlayerInputActions);
  DialogueWidget->InitUI(PlayerInputActions);
}

// * For when animations / sounds are used.
inline void ShowWidget(UUserWidget* Widget) {
  if (FProperty* FUIBehaviourProp = Widget->GetClass()->FindPropertyByName("UIBehaviour")) {
    FUIBehaviour* UIBehaviour = FUIBehaviourProp->ContainerPtrToValuePtr<FUIBehaviour>(Widget);
    check(UIBehaviour->ShowUI);
    return UIBehaviour->ShowUI();
  }

  Widget->SetVisibility(ESlateVisibility::Visible);
}
inline void HideWidget(UUserWidget* Widget) {
  if (FProperty* FUIBehaviourProp = Widget->GetClass()->FindPropertyByName("UIBehaviour")) {
    FUIBehaviour* UIBehaviour = FUIBehaviourProp->ContainerPtrToValuePtr<FUIBehaviour>(Widget);
    check(UIBehaviour->HideUI);
    return UIBehaviour->HideUI();
  }

  Widget->SetVisibility(ESlateVisibility::Collapsed);
}

void ASpgHUD::LeaveHUD() {
  const FInputModeGameOnly InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(false);

  ShowInGameHudWidget();
  SetPlayerNormalFunc();
}

// TODO: Change to use FUIBehaviour and FUIActionable structs.
void ASpgHUD::OpenFocusedMenu(UUserWidget* Widget) {
  for (auto* OpenedWidget : OpenedWidgets) HideWidget(OpenedWidget);
  OpenedWidgets.Empty();

  ShowWidget(Widget);

  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(Widget);

  HideInGameHudWidget();
  SetPlayerFocussedFunc();
}

void ASpgHUD::ShowInGameHudWidget() {
  if (bShowingHud) return InGameHudWidget->RefreshUI();

  InGameHudWidget->SetVisibility(ESlateVisibility::Visible);
  InGameHudWidget->RefreshUI();

  bShowingHud = true;
}
void ASpgHUD::HideInGameHudWidget() {
  InGameHudWidget->SetVisibility(ESlateVisibility::Collapsed);

  bShowingHud = false;
}

void ASpgHUD::OpenPauseMenu(ASaveManager* SaveManager) {
  if (OpenedWidgets.Contains(PauseMenuWidget)) return CloseWidget(PauseMenuWidget);

  PauseMenuWidget->SaveManagerRef = SaveManager;
  PauseMenuWidget->RefreshUI();

  PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(PauseMenuWidget);

  SetPlayerPausedFunc();
}

void ASpgHUD::PlayerCloseTopOpenMenu() {
  if (OpenedWidgets.IsEmpty()) return;

  auto* Widget = OpenedWidgets.Pop();
  HideWidget(Widget);

  if (!OpenedWidgets.IsEmpty()) return;

  // Only relevant for focussed menus.
  if (EarlyCloseWidgetFunc) EarlyCloseWidgetFunc();
  EarlyCloseWidgetFunc = nullptr;

  LeaveHUD();
}

void ASpgHUD::PlayerCloseAllMenus() {
  for (UUserWidget* Widget : OpenedWidgets) HideWidget(Widget);

  if (EarlyCloseWidgetFunc) EarlyCloseWidgetFunc();
  EarlyCloseWidgetFunc = nullptr;

  OpenedWidgets.Empty();
  LeaveHUD();
}

// * Called by the widget themselves.
void ASpgHUD::CloseWidget(UUserWidget* Widget) {
  check(Widget);

  // ? Set visibility in the widget itself?
  HideWidget(Widget);
  OpenedWidgets.RemoveSingle(Widget);
  EarlyCloseWidgetFunc = nullptr;

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

void ASpgHUD::OpenInteractionPopup(FText InteractionText) {
  InteractionPopupWidget->InteractionText->SetText(InteractionText);
  InteractionPopupWidget->SetVisibility(ESlateVisibility::Visible);
}
void ASpgHUD::CloseInteractionPopup() { InteractionPopupWidget->SetVisibility(ESlateVisibility::Collapsed); }

// todo-low: Update.
void ASpgHUD::SetAndOpenInventoryView(UInventoryComponent* PlayerInventory) {
  check(InventoryViewWidget);
  if (OpenedWidgets.Contains(InventoryViewWidget)) return CloseWidget(InventoryViewWidget);

  InventoryViewWidget->InitUI(PlayerInputActions, Store, MarketEconomy, PlayerInventory,
                              [this] { CloseWidget(InventoryViewWidget); });
  InventoryViewWidget->RefreshUI();

  OpenFocusedMenu(InventoryViewWidget);
}

void ASpgHUD::SetAndOpenBuildableDisplay(ABuildable* Buildable) {
  check(BuildableDisplayViewWidget);
  if (OpenedWidgets.Contains(BuildableDisplayViewWidget)) return CloseWidget(BuildableDisplayViewWidget);

  BuildableDisplayViewWidget->InitUI(PlayerInputActions, Buildable, Store,
                                     [this] { CloseWidget(BuildableDisplayViewWidget); });
  BuildableDisplayViewWidget->RefreshUI();

  OpenFocusedMenu(BuildableDisplayViewWidget);
}

void ASpgHUD::SetAndOpenStoreView(const UInventoryComponent* PlayerInventory) {
  check(StoreViewWidget);
  if (OpenedWidgets.Contains(StoreViewWidget)) return CloseWidget(StoreViewWidget);

  StoreViewWidget->InitUI(PlayerInputActions, DayManager, StorePhaseManager, MarketEconomy, Market, StatisticsGen,
                          UpgradeManager, AbilityManager, PlayerInventory, Store, StoreExpansionManager,
                          [this] { CloseWidget(StoreViewWidget); });
  StoreViewWidget->RefreshUI();

  OpenFocusedMenu(StoreViewWidget);
}

void ASpgHUD::SetAndOpenStockDisplay(UStockDisplayComponent* StockDisplay,
                                     UInventoryComponent* DisplayInventory,
                                     UInventoryComponent* PlayerInventory) {
  check(StockDisplayViewWidget);
  if (OpenedWidgets.Contains(StockDisplayViewWidget)) return CloseWidget(StockDisplayViewWidget);

  StockDisplayViewWidget->InitUI(PlayerInputActions, MarketEconomy, Store, StockDisplay, DisplayInventory,
                                 PlayerInventory, [this] { CloseWidget(StockDisplayViewWidget); });
  StockDisplayViewWidget->RefreshUI();

  OpenFocusedMenu(StockDisplayViewWidget);
}

void ASpgHUD::SetAndOpenStoreExpansionsList(std::function<void(EStoreExpansionLevel)> SelectExpansionFunc) {
  if (OpenedWidgets.Contains(StoreExpansionsListWidget)) return CloseWidget(StoreExpansionsListWidget);

  StoreExpansionsListWidget->StoreExpansionManagerRef = StoreExpansionManager;
  StoreExpansionsListWidget->SelectExpansionFunc = [this, SelectExpansionFunc](EStoreExpansionLevel ExpansionLevel) {
    SelectExpansionFunc(ExpansionLevel);

    PlayerCloseAllMenus();
  };
  StoreExpansionsListWidget->RefreshUI();

  ShowWidget(StoreExpansionsListWidget);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(StoreExpansionsListWidget);
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

  ShowWidget(PlayerAndContainerWidget);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(PlayerAndContainerWidget);
}

void ASpgHUD::SetAndOpenNPCStore(UNpcStoreComponent* NpcStoreC,
                                 UInventoryComponent* NPCStoreInventory,
                                 UInventoryComponent* PlayerInventory) {
  check(NpcStoreC && NPCStoreInventory && PlayerInventory);

  NpcStoreViewWidget->InitUI(PlayerInputActions, MarketEconomy, Market, Store, NpcStoreC, NPCStoreInventory,
                             PlayerInventory, [this] { CloseWidget(NpcStoreViewWidget); });
  NpcStoreViewWidget->RefreshUI();

  OpenFocusedMenu(NpcStoreViewWidget);
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
    // if (InventoryViewWidget->IsVisible()) InventoryViewWidget->RefreshInventoryViewUI();
  };
  NegotiationWidget->InitNegotiationUI();

  OpenFocusedMenu(NegotiationWidget);
}

void ASpgHUD::SetAndOpenCutscene(UCutsceneSystem* CutsceneSystem) {
  check(CutsceneWidget);

  CutsceneWidget->CloseThisUI = [this]() {
    CloseWidget(CutsceneWidget);
    SetPlayerNormalFunc();
  };
  OpenFocusedMenu(CutsceneWidget);
  SetPlayerCutsceneFunc();

  CutsceneWidget->InitUI(CutsceneSystem);
}
void ASpgHUD::SkipCutscene() {
  check(CutsceneWidget);
  CutsceneWidget->SkipCutscene();
}

void ASpgHUD::SetAndOpenNewsAndEconomyView() {
  check(NewsAndEconomyViewWidget);
  if (OpenedWidgets.Contains(NewsAndEconomyViewWidget)) return CloseWidget(NewsAndEconomyViewWidget);

  NewsAndEconomyViewWidget->InitUI(PlayerInputActions, DayManager, MarketEconomy, NewsGen,
                                   [this] { CloseWidget(NewsAndEconomyViewWidget); });
  NewsAndEconomyViewWidget->RefreshUI();

  OpenFocusedMenu(NewsAndEconomyViewWidget);
}

void ASpgHUD::SetAndOpenUpgradeView(UUpgradeSelectComponent* UpgradeSelectC) {
  check(UpgradeViewWidget);

  if (OpenedWidgets.Contains(UpgradeViewWidget)) return CloseWidget(UpgradeViewWidget);

  UpgradeViewWidget->InitUI(PlayerInputActions, UpgradeManager, UpgradeSelectC,
                            [this] { CloseWidget(UpgradeViewWidget); });
  UpgradeViewWidget->RefreshUI();

  OpenFocusedMenu(UpgradeViewWidget);
}

void ASpgHUD::SetAndOpenAbilityView() {
  check(AbilityViewWidget);

  if (OpenedWidgets.Contains(AbilityViewWidget)) return CloseWidget(AbilityViewWidget);

  AbilityViewWidget->InitUI(PlayerInputActions, AbilityManager, [this] { CloseWidget(AbilityViewWidget); });
  AbilityViewWidget->RefreshUI();

  OpenFocusedMenu(AbilityViewWidget);
}

void ASpgHUD::SetAndOpenMiniGame(AMiniGameManager* MiniGameManager,
                                 UMiniGameComponent* MiniGameC,
                                 AStore* _Store,
                                 UInventoryComponent* PlayerInventory) {
  UUserWidget* MiniGameWidget = MiniGameManager->GetMiniGameWidget(
      MiniGameC->MiniGameType, _Store, PlayerInventory, [this](UUserWidget* Widget) { CloseWidget(Widget); });

  MiniGameWidget->AddToViewport(10);
  OpenFocusedMenu(MiniGameWidget);
}
void ASpgHUD::StorePhaseTransition(std::function<void()> _FadeInEndFunc) {
  SetPlayerPausedFunc();

  StorePhaseTransitionWidget->FadeInEndFunc = _FadeInEndFunc;
  StorePhaseTransitionWidget->FadeOutEndFunc = [this]() {
    ShowInGameHudWidget();
    SetPlayerNormalFunc();

    StorePhaseTransitionWidget->SetVisibility(ESlateVisibility::Collapsed);
    StorePhaseTransitionWidget->RemoveFromParent();
  };

  StorePhaseTransitionWidget->AddToViewport(100);
  StorePhaseTransitionWidget->SetVisibility(ESlateVisibility::Visible);
}

void ASpgHUD::StartLevelLoadingTransition(std::function<void()> _FadeInEndFunc) {
  SetPlayerPausedFunc();

  LevelLoadingTransitionWidget->FadeInEndFunc = _FadeInEndFunc;

  LevelLoadingTransitionWidget->AddToViewport(100);
  LevelLoadingTransitionWidget->SetVisibility(ESlateVisibility::Visible);
}
void ASpgHUD::EndLevelLoadingTransition(std::function<void()> _FadeOutEndFunc) {
  LevelLoadingTransitionWidget->FadeOutEndFunc = [this, _FadeOutEndFunc = _FadeOutEndFunc]() {
    SetPlayerNormalFunc();

    LevelLoadingTransitionWidget->SetVisibility(ESlateVisibility::Collapsed);
    LevelLoadingTransitionWidget->RemoveFromParent();

    if (_FadeOutEndFunc) _FadeOutEndFunc();
  };
  LevelLoadingTransitionWidget->OnEndLevelLoadingCalled();
}