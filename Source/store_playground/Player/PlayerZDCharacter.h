// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "PlayerZDCharacter.generated.h"

// * Corresponds to different input contexts to control allowed input actions in various states.
UENUM()
enum class EPlayerState : uint8 {
  Normal UMETA(DisplayName = "Normal"),              // * Normal state.
  Cutscene UMETA(DisplayName = "Cutscene"),          // * In a cutscene.
  FocussedMenu UMETA(DisplayName = "FocussedMenu"),  // * In a focussed menu (dialogue, negotiation, etc.).
  Paused UMETA(DisplayName = "Paused"),              // * In a paused state.
};

USTRUCT()
struct FInputActions {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* MoveAction;

  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* OpenPauseMenuAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* CloseTopOpenMenuAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* CloseAllMenusAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* OpenInventoryViewAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* BuildModeAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* InteractAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* OpenNewspaperAction;

  // Will probably turn into a combined "store" view action.
  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* OpenStatisticsAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* OpenStoreExpansionsAction;

  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* AdvanceUIAction;  // * Advance UI (dialogue, negotiation, etc.).

  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* SkipCutsceneAction;  // * Skip cutscene.
};

USTRUCT()
struct FOcclusionCheckData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float LastOcclusionCheckTime;
  UPROPERTY(EditAnywhere)
  float OcclusionCheckFrequency;
  UPROPERTY(EditAnywhere)
  class UMaterialInterface* OcclusionMaterial;
  UPROPERTY(EditAnywhere)
  FName OcclusionTag;
};

USTRUCT()
struct FInteractionData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  float LastInteractionCheckTime;
  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  bool IsInteracting;
  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  float InteractionCheckFrequency;
  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  float InteractionCheckDistance;
  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  float InteractionCheckRadius;
};

UCLASS()
class STORE_PLAYGROUND_API APlayerZDCharacter : public APaperZDCharacter {
  GENERATED_BODY()

public:
  APlayerZDCharacter();

  // * Overrides
  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Character | PlayerState")
  EPlayerState PlayerBehaviourState;
  void ChangePlayerState(EPlayerState NewState);

  UPROPERTY()
  class ASpgHUD* HUD;

  // * Input
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  TMap<EPlayerState, class UInputMappingContext*> InputContexts;
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  FInputActions InputActions;

  // * Input functions
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void Move(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void OpenPauseMenu(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void PlayerCloseTopOpenMenu(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void PlayerCloseAllMenus(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void OpenInventoryView(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void EnterBuildMode(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void OpenNewspaper(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void OpenStatistics(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void OpenStoreExpansions(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void Interact(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void AdvanceUI(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void SkipCutscene(const FInputActionValue& Value);

  // * Const refs.
  UPROPERTY(EditAnywhere, Category = "Character | Const")
  const class AMarket* Market;
  UPROPERTY(EditAnywhere, Category = "Character | Const")
  const class ACustomerAIManager* CustomerAIManager;
  UPROPERTY(EditAnywhere, Category = "Character | Const")
  const class ANewsGen* NewsGen;
  UPROPERTY(EditAnywhere, Category = "Character | Const")
  const class AStatisticsGen* StatisticsGen;

  // * Modifiable refs.
  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class ASaveManager* SaveManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class AStoreExpansionManager* StoreExpansionManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class AStore* Store;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class AStorePhaseManager* StorePhaseManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class ADayManager* DayManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class ALevelManager* LevelManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class AQuestManager* QuestManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class AUpgradeManager* UpgradeManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class AAbilityManager* AbilityManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class AMiniGameManager* MiniGameManager;

  // * Player systems.
  UPROPERTY(EditAnywhere, Category = "Character | Systems")
  class UDialogueSystem* DialogueSystem;
  UPROPERTY(EditAnywhere, Category = "Character | Systems")
  class UNegotiationSystem* NegotiationSystem;
  UPROPERTY(EditAnywhere, Category = "Character | Systems")
  class UCutsceneSystem* CutsceneSystem;

  // * Components
  UPROPERTY(EditAnywhere, Category = "Character | Components")
  class UInventoryComponent* PlayerInventoryComponent;
  UPROPERTY(EditAnywhere, Category = "Character | Components")
  class UTagsComponent* PlayerTagsComponent;
  UPROPERTY(EditAnywhere, Category = "Character | Components")
  class UWidgetComponent* PlayerWidgetComponent;

  // * Classes
  UPROPERTY(EditAnywhere, Category = "Character | Classes")
  TSubclassOf<class ASpawnPoint> SpawnPointClass;

  // * Movement
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
  FVector FacingDirection;

  // * Occlusion Masking
  UPROPERTY(EditAnywhere, Category = "Character | Occlusion")
  FOcclusionCheckData OcclusionCheckData;
  UPROPERTY(EditAnywhere, Category = "Character | Occlusion")
  class APlayerCameraManager* CameraManager;
  UPROPERTY(EditAnywhere, Category = "Character | Occlusion")
  TArray<AActor*> OccludedActors;
  void HandleOcclusion();  // Ticked.

  // * Interaction
  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  FInteractionData InteractionData;
  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  class UInteractionComponent* CurrentInteractableC;
  auto CheckForInteraction() -> bool;  // Ticked for UI.
  auto IsInteractable(const class UInteractionComponent* Interactable) const -> bool;
  void HandleInteraction(class UInteractionComponent* Interactable);

  void SetupNpcInteraction(class USimpleSpriteAnimComponent* SpriteAnimC);
  void SetupCustomerInteraction(class UCustomerAIComponent* CustomerAI, class USimpleSpriteAnimComponent* SpriteAnimC);

  // * Entries into HUDs and menus.
  void EnterBuildableDisplay(class ABuildable* Buildable);
  void EnterStockDisplay(class UStockDisplayComponent* StockDisplayC, class UInventoryComponent* DisplayInventoryC);
  void EnterNpcStore(class UNpcStoreComponent* NpcStoreC, class UInventoryComponent* StoreInventoryC);
  void EnterUpgradeSelect(class UUpgradeSelectComponent* UpgradeSelectC);
  void EnterAbilitySelect();
  void EnterMiniGame(class UMiniGameComponent* MiniGameC);

  void EnterDialogue(class UDialogueComponent* DialogueC, std::function<void()> OnDialogueEndFunc = nullptr);
  void EnterDialogue(const TArray<struct FDialogueData> DialogueDataArr,
                     std::function<void()> OnDialogueEndFunc = nullptr);
  void EnterNegotiation(class UCustomerAIComponent* CustomerAI,
                        class UItemBase* Item = nullptr,
                        bool bIsQuestAssociated = false,
                        class UQuestComponent* QuestComponent = nullptr);
  void EnterQuest(class UQuestComponent* QuestC,
                  class UDialogueComponent* DialogueC,
                  class USimpleSpriteAnimComponent* SpriteAnimC,
                  class UCustomerAIComponent* CustomerAI = nullptr,
                  class UItemBase* Item = nullptr);
  void EnterCutscene(const struct FResolvedCutsceneData ResolvedCutsceneData);
  void ExitCurrentAction();  // * Exit current action (e.g., dialogue, negotiation, etc.).

  void EnterNewLevel(class ULevelChangeComponent* LevelChangeC);
  void LeaveStore();

  // // * SaveManager
  // UPROPERTY(EditAnywhere, Category = "Character | SaveManager")
  // class ASaveManager* SaveManager;
};
