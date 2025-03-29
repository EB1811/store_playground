// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "PlayerZDCharacter.generated.h"

USTRUCT()
struct FInteractionData {
  GENERATED_BODY()

  UPROPERTY()
  float LastInteractionCheckTime;
  UPROPERTY()
  bool IsInteracting;
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

  UPROPERTY()
  class ASpgHUD* HUD;

  // * Input
  // Temp: Move to input manager / a map.
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  class UInputMappingContext* InputMappingContext;

  UPROPERTY(EditAnywhere, Category = "Character | Input")
  class UInputAction* MoveAction;
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  class UInputAction* CloseTopOpenMenuAction;
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  class UInputAction* CloseAllMenusAction;
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  class UInputAction* OpenInventoryViewAction;
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  class UInputAction* BuildModeAction;
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  class UInputAction* InteractAction;
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  class UInputAction* OpenNewspaperAction;
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

  // * Inventory
  UPROPERTY(EditAnywhere, Category = "Character | Inventory")
  class UInventoryComponent* PlayerInventoryComponent;

  // * Interaction
  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  float InteractionCheckFrequency;
  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  float InteractionCheckDistance;
  void Interact(const FInputActionValue& Value);

  // * Buildable
  void EnterBuildableDisplay(class ABuildable* Buildable);
  // * Stock Display
  void EnterStockDisplay(class UStockDisplayComponent* StockDisplayC, class UInventoryComponent* DisplayInventoryC);

  // * Dialogue
  UPROPERTY(EditAnywhere, Category = "Character | Dialogue")
  class UDialogueSystem* DialogueSystem;
  void EnterDialogue(const TArray<struct FDialogueData> DialogueDataArr,
                     std::function<void()> OnDialogueEndFunc = nullptr);
  void ExitDialogue();

  // * Negotiation
  UPROPERTY(EditAnywhere, Category = "Character | Negotiation")
  class UNegotiationSystem* NegotiationSystem;
  void EnterNegotiation(class UCustomerAIComponent* CustomerAI,
                        const class UItemBase* Item = nullptr,
                        bool bIsQuestAssociated = false,
                        class UQuestComponent* QuestComponent = nullptr);

  // * Quest Logic
  void EnterQuest(class UQuestComponent* QuestC,
                  class UDialogueComponent* DialogueC,
                  class UCustomerAIComponent* CustomerAI = nullptr,
                  const class UItemBase* Item = nullptr);

  // * Npc Store
  void EnterNpcStore(class UNpcStoreComponent* NpcStoreC, class UInventoryComponent* StoreInventoryC);

  // * Store
  UPROPERTY(EditAnywhere, Category = "Character | Store")
  class AStore* Store;

  // * Game Store Phase Manager - to control the global game state.
  UPROPERTY(EditAnywhere, Category = "Character | StorePhaseManager")
  class AStorePhaseManager* StorePhaseManager;

  // * Level Manager
  UPROPERTY(EditAnywhere, Category = "Character | LevelManager")
  class ALevelManager* LevelManager;

  UPROPERTY(EditAnywhere, Category = "Character | LevelManager")
  TSubclassOf<class ASpawnPoint> SpawnPointClass;
  void EnterNewLevel(class ULevelChangeComponent* LevelChangeC);

  // * Market
  UPROPERTY(EditAnywhere, Category = "Character | Market")
  class AMarket* Market;

  // * MarketEconomy
  UPROPERTY(EditAnywhere, Category = "Character | MarketEconomy")
  class AMarketEconomy* MarketEconomy;

  // * CustomerAIManager
  UPROPERTY(EditAnywhere, Category = "Character | CustomerAIManager")
  class ACustomerAIManager* CustomerAIManager;

  // * QuestManager
  UPROPERTY(EditAnywhere, Category = "Character | QuestManager")
  class AQuestManager* QuestManager;

  // * NewsGen
  UPROPERTY(EditAnywhere, Category = "Character | NewsGen")
  class ANewsGen* NewsGen;
};
