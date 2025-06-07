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
#include "store_playground/UI/Store/StockDisplayWidget.h"
#include "store_playground/UI/Store/BuildableDisplayWidget.h"
#include "store_playground/UI/Newspaper/NewspaperWidget.h"
#include "store_playground/UI/Statistics/StatisticsWidget.h"
#include "store_playground/UI/Ability/AbilityWidget.h"
#include "store_playground/UI/InGameHud/InGameHudWidget.h"
#include "store_playground/Store/Store.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/UI/Upgrade/UpgradeListWidget.h"
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
  check(BuildableDisplayWidgetClass);
  check(NpcStoreViewWidgetClass);
  check(UNegotiationWidgetClass);
  check(UDialogueWidgetClass);
  check(StockDisplayWidgetClass);
  check(StoreExpansionsListWidgetClass);
  check(NewspaperWidgetClass);
  check(UpgradeSelectWidgetClass);
  check(AbilityWidgetClass);

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

  BuildableDisplayWidget = CreateWidget<UBuildableDisplayWidget>(GetWorld(), BuildableDisplayWidgetClass);
  BuildableDisplayWidget->AddToViewport(10);
  BuildableDisplayWidget->SetVisibility(ESlateVisibility::Collapsed);

  StockDisplayWidget = CreateWidget<UStockDisplayWidget>(GetWorld(), StockDisplayWidgetClass);
  StockDisplayWidget->AddToViewport(10);
  StockDisplayWidget->SetVisibility(ESlateVisibility::Collapsed);

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

  NewspaperWidget = CreateWidget<UNewspaperWidget>(GetWorld(), NewspaperWidgetClass);
  NewspaperWidget->AddToViewport(10);
  NewspaperWidget->SetVisibility(ESlateVisibility::Collapsed);

  StoreViewWidget = CreateWidget<UStoreViewWidget>(GetWorld(), StoreViewWidgetClass);
  StoreViewWidget->AddToViewport(10);
  StoreViewWidget->SetVisibility(ESlateVisibility::Collapsed);

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

void ASpgHUD::SetAndOpenBuildableDisplay(ABuildable* Buildable,
                                         std::function<bool(class ABuildable* Buildable)> BuildStockDisplayFunc,
                                         std::function<bool(class ABuildable* Buildable)> BuildDecorationFunc) {
  check(BuildableDisplayWidget);

  BuildableDisplayWidget->SetBuildable(Buildable);
  BuildableDisplayWidget->BuildStockDisplayFunc = BuildStockDisplayFunc;
  BuildableDisplayWidget->BuildDecorationFunc = BuildDecorationFunc;
  BuildableDisplayWidget->CloseWidgetFunc = [this]() { CloseWidget(BuildableDisplayWidget); };

  ShowWidget(BuildableDisplayWidget);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(BuildableDisplayWidget);
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
                                     UInventoryComponent* PlayerInventory,
                                     std::function<bool(UItemBase*, UInventoryComponent*)> PlayerToDisplayFunc,
                                     std::function<bool(UItemBase*, UInventoryComponent*)> DisplayToPlayerFunc) {
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

  ShowWidget(StockDisplayWidget);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(StockDisplayWidget);
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

void ASpgHUD::SetAndOpenNewspaper(const ANewsGen* NewsGenRef) {
  check(NewspaperWidget);

  if (OpenedWidgets.Contains(NewspaperWidget)) return CloseWidget(NewspaperWidget);

  NewspaperWidget->NewsGenRef = NewsGenRef;
  NewspaperWidget->RefreshNewspaperUI();

  ShowWidget(NewspaperWidget);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(NewspaperWidget);
}

void ASpgHUD::SetAndOpenUpgradeSelect(UUpgradeSelectComponent* UpgradeSelectC, AUpgradeManager* _UpgradeManager) {
  check(UpgradeSelectWidget);

  UpgradeSelectWidget->UpgradeManagerRef = _UpgradeManager;

  UpgradeSelectWidget->SelectUpgradeFunc = [this, _UpgradeManager](FName UpgradeID) {
    _UpgradeManager->SelectUpgrade(UpgradeID);
  };
  UpgradeSelectWidget->SetUpgradeClass(UpgradeSelectC->UpgradeClass, UpgradeSelectC->Title,
                                       UpgradeSelectC->Description);

  OpenFocusedMenu(UpgradeSelectWidget);
}

void ASpgHUD::SetAndOpenAbilitySelect(class AAbilityManager* _AbilityManager) {
  check(AbilityWidget);

  AbilityWidget->AbilityManagerRef = _AbilityManager;
  AbilityWidget->RefreshUI();

  OpenFocusedMenu(AbilityWidget);
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