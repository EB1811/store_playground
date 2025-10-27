// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "Components/AudioComponent.h"
#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "store_playground/Player/InputStructs.h"
#include "PlayerZDCharacter.generated.h"

// * Corresponds to different input contexts to control allowed input actions in various states.
UENUM()
enum class EPlayerState : uint8 {
  Normal UMETA(DisplayName = "Normal"),
  Cutscene UMETA(DisplayName = "Cutscene"),
  FocussedMenu UMETA(DisplayName = "FocussedMenu"),  // * In a focussed menu (dialogue, negotiation, etc.).
  NoControl UMETA(DisplayName = "NoControl"),        // * No control over the character (e.g., during loading).
  Paused UMETA(DisplayName = "Paused"),
  PausedCutscene UMETA(DisplayName = "PausedCutscene"),
  GameOver UMETA(DisplayName = "GameOver"),
};
// * Used by variaous systems to check what the player is currently doing, e.g., in a quest, negotiation, etc.
UENUM()
enum class EPlayerGameActionsState : uint8 {
  None UMETA(DisplayName = "None"),                    // * Not doing anything special.
  InDialogue UMETA(DisplayName = "InDialogue"),        // * In a dialogue.
  InNegotiation UMETA(DisplayName = "InNegotiation"),  // * In a negotiation.
  InQuest UMETA(DisplayName = "InQuest"),              // * In a quest (e.g., fetching item, etc.).
  InTutorial UMETA(DisplayName = "InTutorial"),        // * In a tutorial.
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

USTRUCT()
struct FCameraCinematicsData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float CamInterpSpeed;  // * Moving, etc.

  UPROPERTY(EditAnywhere)
  float CinematicInterpSpeed;  // * Toggle to/from cinematic view.
  UPROPERTY(EditAnywhere)
  FVector OgSpringArmOffset;
  UPROPERTY(EditAnywhere)
  FVector CinematicSpringArmOffset;
  UPROPERTY(EditAnywhere)
  FRotator OgCamRotation;
  UPROPERTY(EditAnywhere)
  FRotator CinematicCamRotation;
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
  UPROPERTY(EditAnywhere, Category = "Character | PlayerState")
  EPlayerGameActionsState GameActionsState;
  void ChangeGameActionsState(EPlayerGameActionsState NewState);

  UPROPERTY()
  class ASpgHUD* HUD;

  // * Input
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  TMap<EPlayerState, class UInputMappingContext*> InputContexts;
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  FInGameInputActions InGameInputActions;
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  FInUIInputActions InUIInputActions;
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  FInCutsceneInputActions InCutsceneInputActions;
  // * Input functions
  // In Game
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void Move(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void StartMove(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void StopMove(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void OpenPauseMenu(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void OpenInventoryView(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void OpenNewspaper(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void OpenStoreView(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void EnterBuildMode(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void Interact(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void CinematicView(const FInputActionValue& Value);
  // In UI
  UFUNCTION(BlueprintCallable)
  void AdvanceUI(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void AdvanceUIHold(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void RetractUIAction(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void QuitUIAction(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void UINumericInputAction(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void UIDirectionalInputAction(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void UISideButton1Action(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void UISideButton2Action(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void UISideButton3Action(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void UISideButton4Action(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void UICycleLeftAction(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void UICycleRightAction(const FInputActionValue& Value);
  // In Cutscene
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void SkipCutscene(const FInputActionValue& Value);

  // * Const refs.
  UPROPERTY(EditAnywhere, Category = "Character | Const")
  class AMarket* Market;

  // * Modifiable refs.
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class AStoreExpansionManager* StoreExpansionManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class AStore* Store;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class AStorePhaseManager* StorePhaseManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class ALevelManager* LevelManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class AQuestManager* QuestManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class ATutorialManager* TutorialManager;
  UPROPERTY(EditAnywhere, Category = "Character | Modifiable")
  class AMiniGameManager* MiniGameManager;

  // * Player systems.
  UPROPERTY(EditAnywhere, Category = "Character | Systems")
  class UDialogueSystem* DialogueSystem;
  UPROPERTY(EditAnywhere, Category = "Character | Systems")
  class UNegotiationSystem* NegotiationSystem;
  UPROPERTY(EditAnywhere, Category = "Character | Systems")
  class UCutsceneSystem* CutsceneSystem;

  // * Camera
  UPROPERTY(EditAnywhere, Category = "Character | Components")
  class USpringArmComponent* SpringArmC;
  UPROPERTY(EditAnywhere, Category = "Character | Components")
  class UCameraComponent* CameraC;

  // * Components
  UPROPERTY(EditAnywhere, Category = "Character | Components")
  class UInventoryComponent* PlayerInventoryComponent;
  UPROPERTY(EditAnywhere, Category = "Character | Components")
  class UTagsComponent* PlayerTagsComponent;  // Flags
  UPROPERTY(EditAnywhere, Category = "Character | Components")
  class UWidgetComponent* PlayerWidgetComponent;

  // * Classes
  UPROPERTY(EditAnywhere, Category = "Character | Classes")
  TSubclassOf<class ASpawnPoint> SpawnPointClass;

  // * Movement
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
  FVector FacingDirection;
  // Movement Audio
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
  UAudioComponent* FootstepAudio;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
  FTimerHandle FootstepAudioTimer;  // * Loops footstep audio.
  UFUNCTION(BlueprintCallable, Category = "Character | Movement")
  void PlayFootstepAudio();
  UPROPERTY(EditAnywhere, Category = "Character | Movement")
  class UPhysicalMaterial* CurrentFootstepPhysMat;
  UPROPERTY(EditAnywhere, Category = "Character | Movement")
  float LastCurrentFootstepPhysMatCheckTime;
  UFUNCTION(Category = "Character | Movement")
  void CheckCurrentFootstepPhysMat();

  // * Occlusion Masking
  UPROPERTY(EditAnywhere, Category = "Character | Occlusion")
  FOcclusionCheckData OcclusionCheckData;
  UPROPERTY(EditAnywhere, Category = "Character | Occlusion")
  class APlayerCameraManager* CameraManager;
  UPROPERTY(EditAnywhere, Category = "Character | Occlusion")
  TArray<AActor*> OccludedActors;
  void HandleOcclusion();  // Ticked.

  // * Cinematics
  UPROPERTY(EditAnywhere, Category = "Character | Cinematics")
  FCameraCinematicsData CameraCinematicsData;
  UPROPERTY(EditAnywhere, Category = "Character | Cinematics")
  bool bInterpCamera;
  UPROPERTY(EditAnywhere, Category = "Character | Cinematics")
  bool bInCinematicView;
  void ToggleCinematicView();
  void InterpCamera(float DeltaTime);  // Ticked.

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

  void EnterDialogue(class UDialogueComponent* DialogueC,
                     std::function<void()> OnDialogueCloseFunc = nullptr,
                     std::function<void()> OnDialogueFinishFunc = nullptr);
  void EnterDialogue(const TArray<struct FDialogueData> DialogueDataArr,
                     std::function<void()> OnDialogueCloseFunc = nullptr,
                     std::function<void()> OnDialogueFinishFunc = nullptr,
                     const FString& _SpeakerName = "NPC");
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

  void Pickup(class UPickupComponent* PickupC);

  void ExitCurrentAction();  // * Exit current action (e.g., dialogue, negotiation, etc.).
  void ResetLocationToSpawnPoint();

  void EnterNewLevel(class ULevelChangeComponent* LevelChangeC);
  void LeaveStore();

  void GameOverReset();

  // // * SaveManager
  // UPROPERTY(EditAnywhere, Category = "Character | SaveManager")
  // class ASaveManager* SaveManager;
};
