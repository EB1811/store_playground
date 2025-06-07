// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"
#include "SPGHUD.generated.h"

// todo Expand as needed.
UENUM()
enum class EHUDState : uint8 {
  InGame UMETA(DisplayName = "InGame"),
  Paused UMETA(DisplayName = "Paused"),
  MainMenu UMETA(DisplayName = "MainMenu")
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
  TSubclassOf<class UPauseMenuWidget> PauseMenuWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> InventoryViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UInteractionDisplayWidget> InteractionDisplayWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> BuildableDisplayWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> StockDisplayWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UStoreExpansionsListWidget> StoreExpansionsListWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> PlayerAndContainerWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UNpcStoreViewWidget> NpcStoreViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> UDialogueWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> UNegotiationWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UCutsceneWidget> CutsceneWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> NewspaperWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> UpgradeSelectWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UAbilityWidget> AbilityWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UStoreViewWidget> StoreViewWidgetClass;

  UPROPERTY(EditAnywhere)
  const class AStatisticsGen* StatisticsGen;
  UPROPERTY(EditAnywhere)
  const class AUpgradeManager* UpgradeManager;
  UPROPERTY(EditAnywhere)
  const class AAbilityManager* AbilityManager;
  UPROPERTY(EditAnywhere)
  const class ANewsGen* NewsGen;
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
  class AStore* Store;
  UPROPERTY(EditAnywhere)
  class AStoreExpansionManager* StoreExpansionManager;

  UPROPERTY()
  EHUDState HUDState;

  std::function<void()> SetPlayerFocussedFunc;  // * Set player state to menu (in control of the player).
  std::function<void()> SetPlayerCutsceneFunc;  // * Set player state to cutscene (not in control of the player).
  std::function<void()> SetPlayerNormalFunc;    // * Set player state to normal (not in menu, etc.).
  std::function<void()> SetPlayerPausedFunc;    // * Set player state to paused (not in menu, etc.).

  UPROPERTY(EditAnywhere)
  FInputActions PlayerInputActions;

  void SetupInitUIStates();  // * Some stuff needs to be set up after all unreal funcs are called.

  void LeaveHUD();                                  // * Leave the current HUD state (main menu, etc.).
  void OpenFocusedMenu(class UUserWidget* Widget);  // * Open a menu in the focused state (dialogue, negotiation, etc.).

  UPROPERTY(EditAnywhere)
  class UInGameHudWidget* InGameHudWidget;
  UPROPERTY(EditAnywhere)
  bool bShowingHud;
  void ShowInGameHudWidget();
  void HideInGameHudWidget();

  UPROPERTY()
  class UPauseMenuWidget* PauseMenuWidget;
  UFUNCTION(BlueprintCallable)
  void OpenPauseMenu(class ASaveManager* SaveManager);

  UPROPERTY(EditAnywhere)
  TArray<class UUserWidget*> OpenedWidgets;
  UFUNCTION(BlueprintCallable)
  void PlayerCloseTopOpenMenu();
  UFUNCTION(BlueprintCallable)
  void PlayerCloseAllMenus();

  void CloseWidget(class UUserWidget* Widget);
  std::function<void()> EarlyCloseWidgetFunc;  // * When leaving a widget early, i.e, before the end of a dialogue, etc.

  void AdvanceUI();  // * Advance the topmost open menu (dialogue, negotiation, etc.).

  UPROPERTY()
  class UInteractionDisplayWidget* InteractionPopupWidget;
  void OpenInteractionPopup(FText InteractionText);
  void CloseInteractionPopup();

  UPROPERTY()
  class UInventoryViewWidget* InventoryViewWidget;
  void SetAndOpenInventoryView(class UInventoryComponent* PlayerInventory);

  UPROPERTY()
  class UBuildableDisplayWidget* BuildableDisplayWidget;
  void SetAndOpenBuildableDisplay(class ABuildable* Buildable,
                                  std::function<bool(class ABuildable* Buildable)> BuildStockDisplayFunc,
                                  std::function<bool(class ABuildable* Buildable)> BuildDecorationFunc);

  UPROPERTY()
  class UStoreViewWidget* StoreViewWidget;
  void SetAndOpenStoreView(const class UInventoryComponent* PlayerInventory);

  UPROPERTY()
  class UStockDisplayWidget* StockDisplayWidget;
  void SetAndOpenStockDisplay(class UStockDisplayComponent* StockDisplay,
                              class UInventoryComponent* DisplayInventory,
                              class UInventoryComponent* PlayerInventory,
                              std::function<bool(class UItemBase*, class UInventoryComponent*)> PlayerToDisplayFunc,
                              std::function<bool(class UItemBase*, class UInventoryComponent*)> DisplayToPlayerFunc);

  UPROPERTY()
  class UStoreExpansionsListWidget* StoreExpansionsListWidget;
  void SetAndOpenStoreExpansionsList(std::function<void(EStoreExpansionLevel)> SelectExpansionFunc);

  UPROPERTY()
  class UPlayerAndContainerWidget* PlayerAndContainerWidget;
  void SetAndOpenContainer(const class UInventoryComponent* PlayerInventory,
                           const class UInventoryComponent* ContainerInventory);

  UPROPERTY()
  class UNpcStoreViewWidget* NpcStoreViewWidget;
  void SetAndOpenNPCStore(class UNpcStoreComponent* NpcStoreC,
                          class UInventoryComponent* NPCStoreInventory,
                          class UInventoryComponent* PlayerInventory);

  UPROPERTY()
  class UDialogueWidget* DialogueWidget;
  void SetAndOpenDialogue(class UDialogueSystem* Dialogue, std::function<void()> OnDialogueEndFunc = nullptr);

  UPROPERTY()
  class UNegotiationWidget* NegotiationWidget;
  void SetAndOpenNegotiation(const class UNegotiationSystem* Negotiation, class UInventoryComponent* PlayerInventoryC);

  UPROPERTY()
  class UCutsceneWidget* CutsceneWidget;
  void SetAndOpenCutscene(class UCutsceneSystem* CutsceneSystem);
  void SkipCutscene();  // * Skips to next cutscene chain, not the whole cutscene.

  UPROPERTY()
  class UNewspaperWidget* NewspaperWidget;
  void SetAndOpenNewspaper(const class ANewsGen* NewsGenRef);

  UPROPERTY()
  class UUpgradeListWidget* UpgradeSelectWidget;
  void SetAndOpenUpgradeSelect(class UUpgradeSelectComponent* UpgradeSelectC, class AUpgradeManager* _UpgradeManager);

  UPROPERTY()
  class UAbilityWidget* AbilityWidget;
  void SetAndOpenAbilitySelect(class AAbilityManager* _AbilityManager);

  void SetAndOpenMiniGame(class AMiniGameManager* MiniGameManager,
                          class UMiniGameComponent* MiniGameC,
                          class AStore* _Store,
                          class UInventoryComponent* PlayerInventory);

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
};