// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
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
  TSubclassOf<class UPauseMenuWidget> PauseMenuWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> InventoryViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> BuildableDisplayWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> StockDisplayWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UStoreExpansionsListWidget> StoreExpansionsListWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> PlayerAndContainerWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> NpcStoreWidgetClass;
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
  TSubclassOf<class UStatisticsWidget> StatisticsWidgetClass;

  UPROPERTY()
  EHUDState HUDState;

  std::function<void()> SetPlayerFocussedFunc;  // * Set player state to menu (in control of the player).
  std::function<void()> SetPlayerCutsceneFunc;  // * Set player state to cutscene (not in control of the player).
  std::function<void()> SetPlayerNormalFunc;    // * Set player state to normal (not in menu, etc.).
  std::function<void()> SetPlayerPausedFunc;    // * Set player state to paused (not in menu, etc.).
  void LeaveHUD();                              // * Leave the current HUD state (main menu, etc.).

  void OpenFocusedMenu(class UUserWidget* Widget);  // * Open a menu in the focused state (dialogue, negotiation, etc.).

  UPROPERTY()
  class UPauseMenuWidget* PauseMenuWidget;
  UFUNCTION(BlueprintCallable, Category = "Widgets")
  void OpenPauseMenu(class ASaveManager* SaveManager);

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TArray<class UUserWidget*> OpenedWidgets;
  UFUNCTION(BlueprintCallable, Category = "Widgets")
  void CloseTopOpenMenu();
  UFUNCTION(BlueprintCallable, Category = "Widgets")
  void CloseAllMenus();

  void CloseWidget(class UUserWidget* Widget);

  void AdvanceUI();  // * Advance the topmost open menu (dialogue, negotiation, etc.).

  UPROPERTY()
  class UInventoryViewWidget* InventoryViewWidget;
  void SetAndOpenInventoryView(class UInventoryComponent* PlayerInventory, class AStore* Store);

  UPROPERTY()
  class UBuildableDisplayWidget* BuildableDisplayWidget;
  void SetAndOpenBuildableDisplay(class ABuildable* Buildable,
                                  std::function<bool(class ABuildable* Buildable)> BuildStockDisplayFunc,
                                  std::function<bool(class ABuildable* Buildable)> BuildDecorationFunc);

  UPROPERTY()
  class UStockDisplayWidget* StockDisplayWidget;
  void SetAndOpenStockDisplay(class UStockDisplayComponent* StockDisplay,
                              class UInventoryComponent* DisplayInventory,
                              class UInventoryComponent* PlayerInventory,
                              std::function<void(class UItemBase*, class UInventoryComponent*)> PlayerToDisplayFunc,
                              std::function<void(class UItemBase*, class UInventoryComponent*)> DisplayToPlayerFunc);

  UPROPERTY()
  class UStoreExpansionsListWidget* StoreExpansionsListWidget;
  void SetAndOpenStoreExpansionsList(const class AStoreExpansionManager* StoreExpansionManager,
                                     std::function<void(EStoreExpansionLevel)> SelectExpansionFunc);

  UPROPERTY()
  class UPlayerAndContainerWidget* PlayerAndContainerWidget;
  void SetAndOpenContainer(const class UInventoryComponent* PlayerInventory,
                           const class UInventoryComponent* ContainerInventory);

  UPROPERTY()
  class UNpcStoreWidget* NpcStoreWidget;
  void SetAndOpenNPCStore(class UInventoryComponent* NPCStoreInventory,
                          class UInventoryComponent* PlayerInventory,
                          std::function<void(class UItemBase*, class UInventoryComponent*)> PlayerToStoreFunc,
                          std::function<void(class UItemBase*, class UInventoryComponent*)> StoreToPlayerFunc);

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
  class UStatisticsWidget* StatisticsWidget;
  void SetAndOpenStatistics(const class AStatisticsGen* StatisticsGenRef);

  UPROPERTY()
  class UUpgradeListWidget* UpgradeSelectWidget;
  void SetAndOpenUpgradeSelect(class UUpgradeSelectComponent* UpgradeSelectC, class AUpgradeManager* UpgradeManager);

  UPROPERTY()
  class UAbilityWidget* AbilityWidget;
  void SetAndOpenAbilitySelect(class AAbilityManager* AbilityManager);

  void SetAndOpenMiniGame(class AMiniGameManager* MiniGameManager,
                          class UMiniGameComponent* MiniGameC,
                          class AStore* Store,
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