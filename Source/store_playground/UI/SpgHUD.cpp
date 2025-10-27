#include "SpgHUD.h"
#include "GameFramework/PlayerController.h"
#include "store_playground/UI/PauseMenu/PauseMenuViewWidget.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Market/Market.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/UI/Inventory/InventoryViewWidget.h"
#include "store_playground/UI/Dialogue/DialogueWidget.h"
#include "store_playground/UI/Negotiation/NegotiationViewWidget.h"
#include "store_playground/UI/NPCStore/NpcStoreViewWidget.h"
#include "store_playground/UI/Store/StockDisplayViewWidget.h"
#include "store_playground/UI/Store/BuildableDisplayViewWidget.h"
#include "store_playground/UI/Newspaper/NewsAndEconomyViewWidget.h"
#include "store_playground/UI/Ability/AbilityViewWidget.h"
#include "store_playground/UI/InGameHud/InGameHudWidget.h"
#include "store_playground/UI/Negotiation/NegotiationViewWidget.h"
#include "store_playground/UI/GameOver/GameOverViewWidget.h"
#include "store_playground/Store/Store.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/UI/UIStructs.h"
#include "store_playground/UI/Upgrade/UpgradeViewWidget.h"
#include "store_playground/UI/Tutorial/TutorialViewWidget.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/Upgrade/UpgradeSelectComponent.h"
#include "store_playground/Minigame/MiniGameComponent.h"
#include "store_playground/Minigame/MiniGameManager.h"
#include "store_playground/UI/Store/StoreExpansion/StoreExpansionsListWidget.h"
#include "store_playground/UI/Transitions/StorePhaseTransitionWidget.h"
#include "store_playground/UI/Transitions/LevelLoadingTransitionWidget.h"
#include "store_playground/UI/Transitions/InitLoadTransitionWidget.h"
#include "store_playground/UI/Cutscene/CutsceneWidget.h"
#include "store_playground/UI/WorldUI/Player/InteractionDisplayWidget.h"
#include "store_playground/UI/NpcStore/NpcStoreViewWidget.h"
#include "store_playground/UI/Store/StoreViewWidget.h"
#include "store_playground/Cutscene/CutsceneSystem.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

ASpgHUD::ASpgHUD() { HUDState = EHUDState::InGame; }

void ASpgHUD::DrawHUD() { Super::DrawHUD(); }

// ? Init uis at the start of the game?
// ? Or have another function that sets up the initial state of each the UI, e.g., references to systems?
// ? This would be for performance reasons.
void ASpgHUD::BeginPlay() {
  Super::BeginPlay();

  check(StorePhaseTransitionWidgetClass);
  check(LevelLoadingTransitionWidgetClass);
  check(InitLoadTransitionWidgetClass);

  StorePhaseTransitionWidget = CreateWidget<UStorePhaseTransitionWidget>(GetWorld(), StorePhaseTransitionWidgetClass);
  LevelLoadingTransitionWidget =
      CreateWidget<ULevelLoadingTransitionWidget>(GetWorld(), LevelLoadingTransitionWidgetClass);
  InitLoadTransitionWidget = CreateWidget<UInitLoadTransitionWidget>(GetWorld(), InitLoadTransitionWidgetClass);

  InitGameStartTransition();  // End transition is called in the game mode after all systems are initialized.

  check(InGameHudWidgetClass);
  check(PauseMenuViewWidgetClass);
  check(InteractionDisplayWidgetClass);
  check(InventoryViewWidgetClass);
  // check(PlayerAndContainerWidgetClass);
  check(BuildableDisplayViewWidgetClass);
  check(NpcStoreViewWidgetClass);
  check(NegotiationViewWidgetClass);
  check(UDialogueWidgetClass);
  check(StockDisplayViewWidgetClass);
  check(StoreExpansionsListWidgetClass);
  check(NewsAndEconomyViewWidgetClass);
  check(UpgradeViewWidgetClass);
  check(AbilityViewWidgetClass);
  check(StoreViewWidgetClass);
  check(TutorialViewWidgetClass);

  PauseMenuViewWidget = CreateWidget<UPauseMenuViewWidget>(GetWorld(), PauseMenuViewWidgetClass);
  PauseMenuViewWidget->AddToViewport(50);
  PauseMenuViewWidget->SetVisibility(ESlateVisibility::Collapsed);

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

  // PlayerAndContainerWidget = CreateWidget<UPlayerAndContainerWidget>(GetWorld(), PlayerAndContainerWidgetClass);
  // PlayerAndContainerWidget->AddToViewport(10);
  // PlayerAndContainerWidget->SetVisibility(ESlateVisibility::Collapsed);

  NpcStoreViewWidget = CreateWidget<UNpcStoreViewWidget>(GetWorld(), NpcStoreViewWidgetClass);
  NpcStoreViewWidget->AddToViewport(10);
  NpcStoreViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  DialogueWidget = CreateWidget<UDialogueWidget>(GetWorld(), UDialogueWidgetClass);
  DialogueWidget->AddToViewport(10);
  DialogueWidget->SetVisibility(ESlateVisibility::Collapsed);

  NegotiationViewWidget = CreateWidget<UNegotiationViewWidget>(GetWorld(), NegotiationViewWidgetClass);
  NegotiationViewWidget->AddToViewport(10);
  NegotiationViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  // CutsceneWidget = CreateWidget<UCutsceneWidget>(GetWorld(), CutsceneWidgetClass);
  // CutsceneWidget->AddToViewport(10);
  // CutsceneWidget->SetVisibility(ESlateVisibility::Collapsed);

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

  TutorialViewWidget = CreateWidget<UTutorialViewWidget>(GetWorld(), TutorialViewWidgetClass);
  TutorialViewWidget->AddToViewport(20);
  TutorialViewWidget->SetVisibility(ESlateVisibility::Collapsed);

  const FInputModeGameOnly InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(false);

  UIShowAnimCompleteEvent.BindDynamic(this, &ASpgHUD::UIShowAnimComplete);
  UIHideAnimCompleteEvent.BindDynamic(this, &ASpgHUD::UIHideAnimComplete);
}

void ASpgHUD::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ASpgHUD::SetupInitUIStates() {
  InGameHudWidget = CreateWidget<UInGameHudWidget>(GetWorld(), InGameHudWidgetClass);
  InGameHudWidget->AddToViewport(0);
  InGameHudWidget->SetVisibility(ESlateVisibility::Hidden);
  InGameHudWidget->UpgradeManager = UpgradeManager;
  InGameHudWidget->DayManager = DayManager;
  InGameHudWidget->StorePhaseManager = StorePhaseManager;
  InGameHudWidget->Store = Store;
  InGameHudWidget->LevelManager = LevelManager;
  // * Input actions are created in SetupPlayerInputComponent, but it runs after BeginPlay, so we need to set them here.
  InGameHudWidget->InitUI(InGameInputActions);
}

void ASpgHUD::LeaveHUD() {
  UE_LOG(LogTemp, Log, TEXT("Leaving HUD state."));

  const FInputModeGameOnly InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(false);

  ShowInGameHudWidget();
  SetPlayerNormalFunc();
}

inline FUIBehaviour* GetUIBehaviour(UUserWidget* Widget) {
  FProperty* FUIBehaviourProp = Widget->GetClass()->FindPropertyByName("UIBehaviour");
  if (!FUIBehaviourProp) return nullptr;

  return FUIBehaviourProp->ContainerPtrToValuePtr<FUIBehaviour>(Widget);
}
void ASpgHUD::ShowWidget(UUserWidget* Widget) {
  if (FUIBehaviour* UIBehaviour = GetUIBehaviour(Widget)) {
    HUDState = EHUDState::PlayingAnim;
    UIShowAnimCompleteFunc = [this]() { HUDState = EHUDState::FocusedMenu; };

    UWidgetAnimation* ShowAnim = UIBehaviour->ShowAnim;
    Widget->UnbindAllFromAnimationFinished(ShowAnim);
    Widget->BindToAnimationFinished(ShowAnim, UIShowAnimCompleteEvent);

    Widget->SetVisibility(ESlateVisibility::Visible);
    Widget->PlayAnimation(ShowAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);

    USoundBase* OpenSound = UIBehaviour->OpenSound;
    if (OpenSound) UGameplayStatics::PlaySound2D(this, OpenSound, 1.0f);

    return;
  }

  Widget->SetVisibility(ESlateVisibility::Visible);
}
void ASpgHUD::HideWidget(UUserWidget* Widget, std::function<void()> PostCloseFunc) {
  if (FUIBehaviour* UIBehaviour = GetUIBehaviour(Widget)) {
    HUDState = EHUDState::PlayingAnim;
    UIHideAnimCompleteFunc = [this, Widget, PostCloseFunc]() {
      HUDState = EHUDState::InGame;
      Widget->SetVisibility(ESlateVisibility::Collapsed);
      if (PostCloseFunc) PostCloseFunc();
    };

    UWidgetAnimation* HideAnim = UIBehaviour->HideAnim;
    Widget->UnbindAllFromAnimationFinished(HideAnim);
    Widget->BindToAnimationFinished(HideAnim, UIHideAnimCompleteEvent);
    Widget->PlayAnimation(HideAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);

    USoundBase* HideSound = UIBehaviour->HideSound;
    if (HideSound) UGameplayStatics::PlaySound2D(this, HideSound, 1.0f);

    return;
  }

  Widget->SetVisibility(ESlateVisibility::Collapsed);
  if (PostCloseFunc) PostCloseFunc();
}

void ASpgHUD::UIShowAnimComplete() {
  if (UIShowAnimCompleteFunc) UIShowAnimCompleteFunc();
}
void ASpgHUD::UIHideAnimComplete() {
  if (UIHideAnimCompleteFunc) UIHideAnimCompleteFunc();
}

void ASpgHUD::OpenFocusedMenu(UUserWidget* Widget) {
  HideInGameHudWidget();

  OpenedWidgets.Add(Widget);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);
  SetPlayerFocussedFunc();

  ShowWidget(Widget);
}
void ASpgHUD::CloseWidget(UUserWidget* Widget) {
  check(Widget);

  HideWidget(Widget, [this, Widget]() {
    OpenedWidgets.RemoveSingle(Widget);
    if (!OpenedWidgets.IsEmpty()) return;
    LeaveHUD();
  });
}
// Callback just for dialogue widget since other widgets are shown on the dialogue's callback.
void ASpgHUD::CloseWidget(UUserWidget* Widget, std::function<void()> PostCloseFunc) {
  check(Widget);

  HideWidget(Widget, [this, Widget, PostCloseFunc]() {
    OpenedWidgets.RemoveSingle(Widget);
    if (PostCloseFunc) PostCloseFunc();

    if (!OpenedWidgets.IsEmpty()) return;
    LeaveHUD();
  });
}

inline FUIActionable* GetUIActionable(UUserWidget* Widget) {
  FProperty* UIActionableProp = Widget->GetClass()->FindPropertyByName("UIActionable");
  if (!UIActionableProp) return nullptr;

  return UIActionableProp->ContainerPtrToValuePtr<FUIActionable>(Widget);
}
void ASpgHUD::AdvanceUI() {
  if (HUDState == EHUDState::PlayingAnim) return;
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FUIActionable* ActionableWidget = GetUIActionable(TopWidget);
  if (!ActionableWidget || !ActionableWidget->AdvanceUI) return;

  ActionableWidget->AdvanceUI();
}
void ASpgHUD::AdvanceUIHold() {
  UE_LOG(LogTemp, Log, TEXT("AdvanceUIHold called in HUD"));
  if (HUDState == EHUDState::PlayingAnim) return;
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FUIActionable* ActionableWidget = GetUIActionable(TopWidget);
  if (!ActionableWidget || !ActionableWidget->AdvanceUIHold) return;

  ActionableWidget->AdvanceUIHold();
}
void ASpgHUD::RetractUIAction() {
  if (HUDState == EHUDState::PlayingAnim) return;
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FUIActionable* ActionableWidget = GetUIActionable(TopWidget);
  if (!ActionableWidget || !ActionableWidget->RetractUI) return CloseWidget(TopWidget);

  ActionableWidget->RetractUI();
}
void ASpgHUD::QuitUIAction() {
  if (HUDState == EHUDState::PlayingAnim) return;
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FUIActionable* ActionableWidget = GetUIActionable(TopWidget);
  if (!ActionableWidget || !ActionableWidget->QuitUI) return CloseWidget(TopWidget);

  ActionableWidget->QuitUI();
}
void ASpgHUD::UINumericInputAction(float Value) {
  if (HUDState == EHUDState::PlayingAnim) return;
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FUIActionable* ActionableWidget = GetUIActionable(TopWidget);
  if (!ActionableWidget || !ActionableWidget->NumericInput) return;

  ActionableWidget->NumericInput(Value);
}
void ASpgHUD::UIDirectionalInputAction(FVector2D Direction) {
  if (HUDState == EHUDState::PlayingAnim) return;
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FUIActionable* ActionableWidget = GetUIActionable(TopWidget);
  if (!ActionableWidget || !ActionableWidget->DirectionalInput) return;

  ActionableWidget->DirectionalInput(Direction);
}
void ASpgHUD::UISideButton1Action() {
  if (HUDState == EHUDState::PlayingAnim) return;
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FUIActionable* ActionableWidget = GetUIActionable(TopWidget);
  if (!ActionableWidget || !ActionableWidget->SideButton1) return;

  ActionableWidget->SideButton1();
}
void ASpgHUD::UISideButton2Action() {
  if (HUDState == EHUDState::PlayingAnim) return;
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FUIActionable* ActionableWidget = GetUIActionable(TopWidget);
  if (!ActionableWidget || !ActionableWidget->SideButton2) return;

  ActionableWidget->SideButton2();
}
void ASpgHUD::UISideButton3Action() {
  if (HUDState == EHUDState::PlayingAnim) return;
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FUIActionable* ActionableWidget = GetUIActionable(TopWidget);
  if (!ActionableWidget || !ActionableWidget->SideButton3) return;

  ActionableWidget->SideButton3();
}
void ASpgHUD::UISideButton4Action() {
  if (HUDState == EHUDState::PlayingAnim) return;
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FUIActionable* ActionableWidget = GetUIActionable(TopWidget);
  if (!ActionableWidget || !ActionableWidget->SideButton4) return;

  ActionableWidget->SideButton4();
}
void ASpgHUD::UICycleLeftAction() {
  if (HUDState == EHUDState::PlayingAnim) return;
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FUIActionable* ActionableWidget = GetUIActionable(TopWidget);
  if (!ActionableWidget || !ActionableWidget->CycleLeft) return;

  ActionableWidget->CycleLeft();
}
void ASpgHUD::UICycleRightAction() {
  if (HUDState == EHUDState::PlayingAnim) return;
  if (OpenedWidgets.IsEmpty()) return;

  UUserWidget* TopWidget = OpenedWidgets.Last();
  FUIActionable* ActionableWidget = GetUIActionable(TopWidget);
  if (!ActionableWidget || !ActionableWidget->CycleRight) return;

  ActionableWidget->CycleRight();
}

void ASpgHUD::ShowInGameHudWidget() {
  InGameHudWidget->RefreshUI();
  if (bShowingHud) return;

  InGameHudWidget->Show();

  bShowingHud = true;
}
void ASpgHUD::HideInGameHudWidget() {
  if (!bShowingHud) return;

  InGameHudWidget->Hide();

  bShowingHud = false;
}

void ASpgHUD::NotifyUpgradePointsGained() { InGameHudWidget->NotifyUpgradePointsGained(); }

void ASpgHUD::OpenPauseMenuView() {
  if (OpenedWidgets.Contains(PauseMenuViewWidget)) return CloseWidget(PauseMenuViewWidget);

  PauseMenuViewWidget->InitUI(InUIInputActions, SettingsManager, SaveManager, [this] {
    CloseWidget(PauseMenuViewWidget);
    SetPlayerNormalFunc();
  });
  PauseMenuViewWidget->RefreshUI();

  PauseMenuViewWidget->SetVisibility(ESlateVisibility::Visible);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  OpenedWidgets.Add(PauseMenuViewWidget);

  // SetPlayerPausedFunc();
}

// todo-low: remove.
void ASpgHUD::OpenInteractionPopup(FText InteractionText) {
  InteractionPopupWidget->InteractionText->SetText(InteractionText);
  InteractionPopupWidget->SetVisibility(ESlateVisibility::Visible);
}
void ASpgHUD::CloseInteractionPopup() {
  if (InteractionPopupWidget->GetVisibility() == ESlateVisibility::Collapsed) return;
  InteractionPopupWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void ASpgHUD::SetAndOpenInventoryView(UInventoryComponent* PlayerInventory) {
  check(InventoryViewWidget);
  if (OpenedWidgets.Contains(InventoryViewWidget)) return CloseWidget(InventoryViewWidget);

  InventoryViewWidget->InitUI(InUIInputActions, StatisticsGen, Store, MarketEconomy, PlayerInventory,
                              [this] { CloseWidget(InventoryViewWidget); });
  InventoryViewWidget->RefreshUI();

  OpenFocusedMenu(InventoryViewWidget);
}

void ASpgHUD::SetAndOpenBuildableDisplay(ABuildable* Buildable) {
  check(BuildableDisplayViewWidget);
  if (OpenedWidgets.Contains(BuildableDisplayViewWidget)) return CloseWidget(BuildableDisplayViewWidget);

  BuildableDisplayViewWidget->InitUI(InUIInputActions, Buildable, Store,
                                     [this] { CloseWidget(BuildableDisplayViewWidget); });
  BuildableDisplayViewWidget->RefreshUI();

  OpenFocusedMenu(BuildableDisplayViewWidget);
}

void ASpgHUD::SetAndOpenStoreView(const UInventoryComponent* PlayerInventory) {
  check(StoreViewWidget);
  if (OpenedWidgets.Contains(StoreViewWidget)) return CloseWidget(StoreViewWidget);

  StoreViewWidget->InitUI(InUIInputActions, LevelManager, DayManager, StorePhaseManager, MarketEconomy, Market,
                          UpgradeManager, AbilityManager, PlayerInventory, StatisticsGen, Store, StoreExpansionManager,
                          [this] { CloseWidget(StoreViewWidget); });
  StoreViewWidget->RefreshUI();

  OpenFocusedMenu(StoreViewWidget);
}

void ASpgHUD::SetAndOpenStockDisplay(UStockDisplayComponent* StockDisplay,
                                     UInventoryComponent* DisplayInventory,
                                     UInventoryComponent* PlayerInventory) {
  check(StockDisplayViewWidget);
  if (OpenedWidgets.Contains(StockDisplayViewWidget)) return CloseWidget(StockDisplayViewWidget);

  StockDisplayViewWidget->InitUI(InUIInputActions, MarketEconomy, StatisticsGen, Store, StockDisplay, DisplayInventory,
                                 PlayerInventory, [this] { CloseWidget(StockDisplayViewWidget); });
  StockDisplayViewWidget->RefreshUI();

  OpenFocusedMenu(StockDisplayViewWidget);
}

void ASpgHUD::SetAndOpenContainer(const UInventoryComponent* PlayerInventory,
                                  const UInventoryComponent* ContainerInventory) {}

void ASpgHUD::SetAndOpenNPCStore(UNpcStoreComponent* NpcStoreC,
                                 UInventoryComponent* NPCStoreInventory,
                                 UInventoryComponent* PlayerInventory) {
  check(NpcStoreC && NPCStoreInventory && PlayerInventory);

  NpcStoreViewWidget->InitUI(InUIInputActions, MarketEconomy, Market, StatisticsGen, Store, NpcStoreC,
                             NPCStoreInventory, PlayerInventory, [this] { CloseWidget(NpcStoreViewWidget); });
  NpcStoreViewWidget->RefreshUI();

  OpenFocusedMenu(NpcStoreViewWidget);
}

void ASpgHUD::SetAndOpenDialogue(UDialogueSystem* Dialogue,
                                 std::function<void()> OnDialogueCloseFunc,
                                 std::function<void()> OnDialogueFinishedFunc) {
  auto CloseDialogueFunc = [this, OnDialogueCloseFunc, OnDialogueFinishedFunc](bool bDialogueFinished) {
    if (!bDialogueFinished || !OnDialogueFinishedFunc) {
      CloseWidget(DialogueWidget);
      if (OnDialogueCloseFunc) OnDialogueCloseFunc();
      return;
    }

    HideWidget(DialogueWidget);
    OpenedWidgets.RemoveSingle(DialogueWidget);
    if (OnDialogueCloseFunc) OnDialogueCloseFunc();
    OnDialogueFinishedFunc();

    if (!OpenedWidgets.IsEmpty()) return;
    LeaveHUD();
  };
  DialogueWidget->InitUI(InUIInputActions, Dialogue, CloseDialogueFunc);

  OpenFocusedMenu(DialogueWidget);
}

void ASpgHUD::SetAndOpenNegotiation(UNegotiationSystem* NegotiationSystem,
                                    UDialogueSystem* DialogueSystem,
                                    UInventoryComponent* PlayerInventoryC) {
  check(NegotiationViewWidget);
  if (OpenedWidgets.Contains(NegotiationViewWidget)) return CloseWidget(NegotiationViewWidget);

  NegotiationViewWidget->InitUI(InUIInputActions, AbilityManager, Store, MarketEconomy, StatisticsGen, PlayerInventoryC,
                                NegotiationSystem, DialogueSystem, [this] { CloseWidget(NegotiationViewWidget); });
  NegotiationViewWidget->RefreshUI();

  OpenFocusedMenu(NegotiationViewWidget);
}

void ASpgHUD::SetAndOpenCutsceneDialogue(UDialogueSystem* Dialogue, std::function<void()> OnDialogueCloseFunc) {
  auto CloseDialogueFunc = [this, OnDialogueCloseFunc](bool bDialogueFinished) {
    HideWidget(DialogueWidget, OnDialogueCloseFunc);

    OpenedWidgets.RemoveSingle(DialogueWidget);
    const FInputModeGameOnly InputMode;
    GetOwningPlayerController()->SetInputMode(InputMode);
    GetOwningPlayerController()->SetShowMouseCursor(false);
  };
  DialogueWidget->InitUI(InCutsceneInputActions, Dialogue, CloseDialogueFunc);

  OpenedWidgets.Add(DialogueWidget);
  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);

  ShowWidget(DialogueWidget);
}
void ASpgHUD::SetAndOpenCutscene(UCutsceneSystem* CutsceneSystem) {}
void ASpgHUD::SkipCutscene() {}

void ASpgHUD::SetAndOpenNewsAndEconomyView() {
  check(NewsAndEconomyViewWidget);
  if (OpenedWidgets.Contains(NewsAndEconomyViewWidget)) return CloseWidget(NewsAndEconomyViewWidget);

  NewsAndEconomyViewWidget->InitUI(InUIInputActions, DayManager, MarketEconomy, StatisticsGen, NewsGen,
                                   [this] { CloseWidget(NewsAndEconomyViewWidget); });
  NewsAndEconomyViewWidget->RefreshUI();

  OpenFocusedMenu(NewsAndEconomyViewWidget);
}

void ASpgHUD::SetAndOpenUpgradeView(UUpgradeSelectComponent* UpgradeSelectC) {
  check(UpgradeViewWidget);
  if (OpenedWidgets.Contains(UpgradeViewWidget)) return CloseWidget(UpgradeViewWidget);

  UpgradeViewWidget->InitUI(InUIInputActions, UpgradeManager, UpgradeSelectC,
                            [this] { CloseWidget(UpgradeViewWidget); });
  UpgradeViewWidget->RefreshUI();

  OpenFocusedMenu(UpgradeViewWidget);
}

void ASpgHUD::SetAndOpenAbilityView() {
  check(AbilityViewWidget);
  if (OpenedWidgets.Contains(AbilityViewWidget)) return CloseWidget(AbilityViewWidget);

  AbilityViewWidget->InitUI(InUIInputActions, Store, AbilityManager, [this] { CloseWidget(AbilityViewWidget); });
  AbilityViewWidget->RefreshUI();

  OpenFocusedMenu(AbilityViewWidget);
}

void ASpgHUD::SetAndOpenTutorialView(TArray<FUITutorialStep> TutorialSteps) {
  check(TutorialViewWidget);

  TutorialViewWidget->InitUI(InUIInputActions, TutorialSteps, [this] {
    CloseWidget(TutorialViewWidget);
    SetGameActionNoneFunc();
  });
  TutorialViewWidget->RefreshUI();

  OpenFocusedMenu(TutorialViewWidget);
  SetGameActionTutorialFunc();
}

void ASpgHUD::SetAndOpenMiniGame(AMiniGameManager* MiniGameManager,
                                 UMiniGameComponent* MiniGameC,
                                 AStore* _Store,
                                 UInventoryComponent* PlayerInventory) {
  UUserWidget* MiniGameWidget = MiniGameManager->GetMiniGameWidget(
      InUIInputActions, MiniGameC, PlayerInventory, [this](UUserWidget* Widget) { CloseWidget(Widget); });

  MiniGameWidget->AddToViewport(10);
  OpenFocusedMenu(MiniGameWidget);
}

void ASpgHUD::StorePhaseTransition(std::function<void()> _FadeInEndFunc) {
  SetPlayerNoControlFunc();
  HideInGameHudWidget();

  StorePhaseTransitionWidget->FadeInEndFunc = _FadeInEndFunc;
  StorePhaseTransitionWidget->FadeOutEndFunc = [this]() {
    StorePhaseTransitionWidget->SetVisibility(ESlateVisibility::Collapsed);
    StorePhaseTransitionWidget->RemoveFromParent();

    if (HUDState == EHUDState::GameOver) return;

    ShowInGameHudWidget();
    SetPlayerNormalFunc();
  };

  StorePhaseTransitionWidget->AddToViewport(100);
  StorePhaseTransitionWidget->SetVisibility(ESlateVisibility::Visible);
}

void ASpgHUD::SetAndOpenGameOverView() {
  HUDState = EHUDState::GameOver;
  HideInGameHudWidget();

  GameOverViewWidget = CreateWidget<UGameOverViewWidget>(GetWorld(), GameOverViewWidgetClass);
  GameOverViewWidget->AddToViewport(50);
  GameOverViewWidget->InitUI(InUIInputActions);
  GameOverViewWidget->SetVisibility(ESlateVisibility::Visible);

  const FInputModeGameAndUI InputMode;
  GetOwningPlayerController()->SetInputMode(InputMode);
  GetOwningPlayerController()->SetShowMouseCursor(true);
}

void ASpgHUD::StartLevelLoadingTransition(std::function<void()> _FadeInEndFunc) {
  SetPlayerNoControlFunc();

  LevelLoadingTransitionWidget->FadeInEndFunc = _FadeInEndFunc;

  LevelLoadingTransitionWidget->AddToViewport(100);
  LevelLoadingTransitionWidget->SetVisibility(ESlateVisibility::Visible);
}
void ASpgHUD::EndLevelLoadingTransition(std::function<void()> _FadeOutEndFunc) {
  LevelLoadingTransitionWidget->FadeOutEndFunc = [this, _FadeOutEndFunc = _FadeOutEndFunc]() {
    if (OpenedWidgets.IsEmpty()) SetPlayerNormalFunc();
    else SetPlayerFocussedFunc();

    LevelLoadingTransitionWidget->SetVisibility(ESlateVisibility::Collapsed);
    LevelLoadingTransitionWidget->RemoveFromParent();

    if (_FadeOutEndFunc) _FadeOutEndFunc();
  };
  LevelLoadingTransitionWidget->OnEndLevelLoadingCalled();
}

void ASpgHUD::StartGameLoadTransition(std::function<void()> _FadeInEndFunc) {
  LevelLoadingTransitionWidget->FadeInEndFunc = _FadeInEndFunc;

  LevelLoadingTransitionWidget->AddToViewport(100);
  LevelLoadingTransitionWidget->SetVisibility(ESlateVisibility::Visible);
}

void ASpgHUD::InitGameStartTransition() {
  InitLoadTransitionWidget->AddToViewport(100);
  InitLoadTransitionWidget->SetVisibility(ESlateVisibility::Visible);
}
void ASpgHUD::InitGameEndTransition(bool bIsNewGame) {
  InitLoadTransitionWidget->FadeOutEndFunc = [this]() {
    InitLoadTransitionWidget->SetVisibility(ESlateVisibility::Collapsed);
    InitLoadTransitionWidget->RemoveFromParent();
    InitLoadTransitionWidget->SetRenderOpacity(1.0f);
  };
  if (bIsNewGame) InitLoadTransitionWidget->NewGameFadeOut();
  else InitLoadTransitionWidget->FadeOut();
}