// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/WidgetAnimationEvents.h"
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"
#include "store_playground/Tutorial/TutorialStructs.h"
#include "SPGHUD.generated.h"

UENUM()
enum class EHUDState : uint8 {
  InGame UMETA(DisplayName = "InGame"),
  Paused UMETA(DisplayName = "Paused"),
  FocusedMenu UMETA(DisplayName = "FocusedMenu"),
  PlayingAnim UMETA(DisplayName = "PlayingAnimation"),
  GameOver UMETA(DisplayName = "GameOver"),
};

UCLASS()
class STORE_PLAYGROUND_API ASpgHUD : public AHUD {
  GENERATED_BODY()

public:
  ASpgHUD();
  virtual void DrawHUD() override;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UInGameHudWidget> InGameHudWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UPauseMenuViewWidget> PauseMenuViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> InventoryViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UInteractionDisplayWidget> InteractionDisplayWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UBuildableDisplayViewWidget> BuildableDisplayViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UStockDisplayViewWidget> StockDisplayViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UStoreExpansionsListWidget> StoreExpansionsListWidgetClass;
  // UPROPERTY(EditAnywhere, Category = "Widgets")
  // TSubclassOf<class UUserWidget> PlayerAndContainerWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UNpcStoreViewWidget> NpcStoreViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> UDialogueWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UNegotiationViewWidget> NegotiationViewWidgetClass;
  // UPROPERTY(EditAnywhere, Category = "Widgets")
  // TSubclassOf<class UCutsceneWidget> CutsceneWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UNewsAndEconomyViewWidget> NewsAndEconomyViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUpgradeViewWidget> UpgradeViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UAbilityViewWidget> AbilityViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UStoreViewWidget> StoreViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UTutorialViewWidget> TutorialViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UGameOverViewWidget> GameOverViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UGameOverViewWidget> DemoGameOverViewWidgetClass;

  UPROPERTY(EditAnywhere)
  const class ADayManager* DayManager;
  UPROPERTY(EditAnywhere)
  const class AStorePhaseManager* StorePhaseManager;
  UPROPERTY(EditAnywhere)
  const class ALevelManager* LevelManager;
  UPROPERTY(EditAnywhere)
  const class AMarketEconomy* MarketEconomy;
  UPROPERTY(EditAnywhere)
  const class AMarket* Market;
  UPROPERTY(EditAnywhere)
  const class ACustomerAIManager* CustomerAIManager;

  UPROPERTY(EditAnywhere)
  class ASettingsManager* SettingsManager;
  UPROPERTY(EditAnywhere)
  class ASaveManager* SaveManager;
  UPROPERTY(EditAnywhere)
  class ADebtManager* DebtManager;
  UPROPERTY(EditAnywhere)
  class AStatisticsGen* StatisticsGen;
  UPROPERTY(EditAnywhere)
  class AUpgradeManager* UpgradeManager;
  UPROPERTY(EditAnywhere)
  class AAbilityManager* AbilityManager;
  UPROPERTY(EditAnywhere)
  class ANewsGen* NewsGen;
  UPROPERTY(EditAnywhere)
  class AStore* Store;
  UPROPERTY(EditAnywhere)
  class AStoreExpansionManager* StoreExpansionManager;

  UPROPERTY()
  EHUDState HUDState;

  std::function<void()> SetPlayerFocussedFunc;   // * Set player state to menu (in control of the player).
  std::function<void()> SetPlayerNormalFunc;     // * Set player state to normal (not in menu, etc.).
  std::function<void()> SetPlayerNoControlFunc;  // * Set player state to no control (i.e, loading, etc.).
  std::function<void()> SetPlayerPausedFunc;     // * Set player state to paused (not in menu, etc.).

  TFunction<void()> SetGameActionTutorialFunc;
  TFunction<void()> SetGameActionNoneFunc;

  UPROPERTY(EditAnywhere)
  FInGameInputActions InGameInputActions;
  UPROPERTY(EditAnywhere)
  FInUIInputActions InUIInputActions;
  UPROPERTY(EditAnywhere)
  FInCutsceneInputActions InCutsceneInputActions;

  void SetupInitUIStates();  // * Some stuff needs to be set up after all unreal funcs are called.

  void LeaveHUD();  // * Leave the current HUD state (main menu, etc.).

  void ShowWidget(class UUserWidget* Widget);
  void HideWidget(class UUserWidget* Widget, std::function<void()> PostCloseFunc = nullptr);
  FWidgetAnimationDynamicEvent UIShowAnimCompleteEvent;
  UFUNCTION()
  void UIShowAnimComplete();
  std::function<void()> UIShowAnimCompleteFunc;
  FWidgetAnimationDynamicEvent UIHideAnimCompleteEvent;
  UFUNCTION()
  void UIHideAnimComplete();
  std::function<void()> UIHideAnimCompleteFunc;

  UPROPERTY(EditAnywhere)
  TArray<class UUserWidget*> OpenedWidgets;

  void OpenFocusedMenu(class UUserWidget* Widget);
  void CloseWidget(class UUserWidget* Widget);
  void CloseWidget(class UUserWidget* Widget, std::function<void()> PostCloseFunc);

  void AdvanceUI();
  void AdvanceUIHold();
  void RetractUIAction();
  void QuitUIAction();
  void UINumericInputAction(float Value);
  void UIDirectionalInputAction(FVector2D Direction);
  void UISideButton1Action();
  void UISideButton2Action();
  void UISideButton3Action();
  void UISideButton4Action();
  void UICycleLeftAction();
  void UICycleRightAction();

  UPROPERTY(EditAnywhere)
  class UInGameHudWidget* InGameHudWidget;
  UPROPERTY(EditAnywhere)
  bool bShowingHud;
  void ShowInGameHudWidget();
  void HideInGameHudWidget();

  void NotifyUpgradePointsGained();

  UPROPERTY()
  class UPauseMenuViewWidget* PauseMenuViewWidget;
  UFUNCTION(BlueprintCallable)
  void OpenPauseMenuView();

  UPROPERTY()
  class UInteractionDisplayWidget* InteractionPopupWidget;
  void OpenInteractionPopup(FText InteractionText);
  void CloseInteractionPopup();

  UPROPERTY()
  class UInventoryViewWidget* InventoryViewWidget;
  void SetAndOpenInventoryView(class UInventoryComponent* PlayerInventory);

  UPROPERTY()
  class UBuildableDisplayViewWidget* BuildableDisplayViewWidget;
  void SetAndOpenBuildableDisplay(class ABuildable* Buildable);

  UPROPERTY()
  class UStoreViewWidget* StoreViewWidget;
  void SetAndOpenStoreView(const class UInventoryComponent* PlayerInventory);

  UPROPERTY()
  class UStockDisplayViewWidget* StockDisplayViewWidget;
  void SetAndOpenStockDisplay(class UStockDisplayComponent* StockDisplay,
                              class UInventoryComponent* DisplayInventory,
                              class UInventoryComponent* PlayerInventory);

  // UPROPERTY()
  // class UPlayerAndContainerWidget* PlayerAndContainerWidget;
  void SetAndOpenContainer(const class UInventoryComponent* PlayerInventory,
                           const class UInventoryComponent* ContainerInventory);

  UPROPERTY()
  class UNpcStoreViewWidget* NpcStoreViewWidget;
  void SetAndOpenNPCStore(class UNpcStoreComponent* NpcStoreC,
                          class UInventoryComponent* NPCStoreInventory,
                          class UInventoryComponent* PlayerInventory);

  UPROPERTY()
  class UDialogueWidget* DialogueWidget;
  void SetAndOpenDialogue(class UDialogueSystem* Dialogue,
                          std::function<void()> OnDialogueCloseFunc = nullptr,
                          std::function<void()> OnDialogueFinishedFunc = nullptr);

  UPROPERTY()
  class UNegotiationViewWidget* NegotiationViewWidget;
  void SetAndOpenNegotiation(class UNegotiationSystem* NegotiationSystem,
                             class UDialogueSystem* DialogueSystem,
                             class UInventoryComponent* PlayerInventoryC);

  // UPROPERTY()
  // class UCutsceneWidget* CutsceneWidget;
  void SetAndOpenCutsceneDialogue(class UDialogueSystem* Dialogue, std::function<void()> OnDialogueCloseFunc = nullptr);
  void SetAndOpenCutscene(class UCutsceneSystem* CutsceneSystem);
  void SkipCutscene();  // * Skips to next cutscene chain, not the whole cutscene.

  UPROPERTY()
  class UNewsAndEconomyViewWidget* NewsAndEconomyViewWidget;
  void SetAndOpenNewsAndEconomyView();

  UPROPERTY()
  class UUpgradeViewWidget* UpgradeViewWidget;
  void SetAndOpenUpgradeView(class UUpgradeSelectComponent* UpgradeSelectC);

  UPROPERTY()
  class UAbilityViewWidget* AbilityViewWidget;
  void SetAndOpenAbilityView();

  UPROPERTY()
  class UTutorialViewWidget* TutorialViewWidget;
  void SetAndOpenTutorialView(TArray<FUITutorialStep> TutorialSteps);

  void SetAndOpenMiniGame(class AMiniGameManager* MiniGameManager,
                          class UMiniGameComponent* MiniGameC,
                          class AStore* _Store,
                          class UInventoryComponent* PlayerInventory);

  UPROPERTY()
  class UGameOverViewWidget* GameOverViewWidget;
  void SetAndOpenGameOverView();
  UPROPERTY()
  class UGameOverViewWidget* DemoGameOverViewWidget;
  void SetAndOpenDemoGameOverView();

  // * Transitions
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UStorePhaseTransitionWidget> StorePhaseTransitionWidgetClass;
  UPROPERTY()
  class UStorePhaseTransitionWidget* StorePhaseTransitionWidget;
  void StorePhaseTransition(std::function<void()> _FadeInEndFunc);

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class ULevelLoadingTransitionWidget> LevelLoadingTransitionWidgetClass;
  UPROPERTY()
  class ULevelLoadingTransitionWidget* LevelLoadingTransitionWidget;
  void StartLevelLoadingTransition(std::function<void()> _FadeInEndFunc);
  void EndLevelLoadingTransition(std::function<void()> _FadeOutEndFunc = nullptr);

  void StartGameLoadTransition(std::function<void()> _FadeInEndFunc);

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UInitLoadTransitionWidget> InitLoadTransitionWidgetClass;
  UPROPERTY()
  class UInitLoadTransitionWidget* InitLoadTransitionWidget;
  void InitGameStartTransition();
  void InitGameEndTransition(bool bIsNewGame = false);
};