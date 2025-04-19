// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "PlayerZDCharacter.generated.h"

UENUM()
enum class EPlayerState : uint8 {
  Normal UMETA(DisplayName = "Normal"),              // * Normal state.
  Cutscene UMETA(DisplayName = "Cutscene"),          // * In a cutscene.
  FocussedMenu UMETA(DisplayName = "FocussedMenu"),  // * In a focussed menu (dialogue, negotiation, etc.).
};

USTRUCT()
struct FInputActions {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* MoveAction;
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

  UPROPERTY(EditAnywhere, Category = "Input")
  class UInputAction* AdvanceUIAction;  // * Advance UI (dialogue, negotiation, etc.).
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
  void CloseTopOpenMenu(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void CloseAllMenus(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void OpenInventoryView(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void EnterBuildMode(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void OpenNewspaper(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void TryInteract(const FInputActionValue& Value);
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  FInteractionData InteractionData;
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void AdvanceUI(const FInputActionValue& Value);

  // * Const refs.
  UPROPERTY(EditAnywhere, Category = "Character | Const")
  const class AMarket* Market;
  UPROPERTY(EditAnywhere, Category = "Character | Const")
  const class ACustomerAIManager* CustomerAIManager;
  UPROPERTY(EditAnywhere, Category = "Character | Const")
  const class ANewsGen* NewsGen;

  // * Modifiable refs.
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

  // * Components
  UPROPERTY(EditAnywhere, Category = "Character | Components")
  class UInventoryComponent* PlayerInventoryComponent;

  // * Classes
  UPROPERTY(EditAnywhere, Category = "Character | Classes")
  TSubclassOf<class ASpawnPoint> SpawnPointClass;

  void HandleInteraction(const class UInteractionComponent* Interactable);

  // * Entries into HUDs and menus.
  void EnterBuildableDisplay(class ABuildable* Buildable);
  void EnterStockDisplay(class UStockDisplayComponent* StockDisplayC, class UInventoryComponent* DisplayInventoryC);
  void EnterNpcStore(class UNpcStoreComponent* NpcStoreC, class UInventoryComponent* StoreInventoryC);
  void EnterUpgradeSelect(class UUpgradeSelectComponent* UpgradeSelectC);
  void EnterAbilitySelect();
  void EnterMiniGame(class UMiniGameComponent* MiniGameC);

  void EnterDialogue(const TArray<struct FDialogueData> DialogueDataArr,
                     std::function<void()> OnDialogueEndFunc = nullptr);
  void EnterNegotiation(class UCustomerAIComponent* CustomerAI,
                        const class UItemBase* Item = nullptr,
                        bool bIsQuestAssociated = false,
                        class UQuestComponent* QuestComponent = nullptr);
  void EnterQuest(class UQuestComponent* QuestC,
                  class UDialogueComponent* DialogueC,
                  class UCustomerAIComponent* CustomerAI = nullptr,
                  const class UItemBase* Item = nullptr);
  void EnterCutscene(const TArray<struct FDialogueData> DialogueDataArr);

  void EnterNewLevel(class ULevelChangeComponent* LevelChangeC);

  // // * SaveManager
  // UPROPERTY(EditAnywhere, Category = "Character | SaveManager")
  // class ASaveManager* SaveManager;
};
