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

  //* Overrides
  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY()
  class ASpgHUD* HUD;

  //* Input
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
  class UInputAction* InteractAction;
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void Move(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void CloseTopOpenMenu(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void CloseAllMenus(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void OpenInventoryView(const FInputActionValue& Value);

  //* Inventory
  UPROPERTY(EditAnywhere, Category = "Character | Inventory")
  class UInventoryComponent* PlayerInventoryComponent;

  //* Interaction
  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  float InteractionCheckFrequency;
  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  float InteractionCheckDistance;
  void Interact(const FInputActionValue& Value);

  //* Dialogue
  UPROPERTY(EditAnywhere, Category = "Character | Dialogue")
  class UDialogueSystem* DialogueSystem;
  void EnterDialogue(const TArray<struct FDialogueData> DialogueDataArr,
                     std::function<void()> OnDialogueEndFunc = nullptr);
  void ExitDialogue();

  //* Negotiation
  UPROPERTY(EditAnywhere, Category = "Character | Negotiation")
  class UNegotiationSystem* NegotiationSystem;
  void EnterNegotiation(const class UItemBase* Item, class UCustomerAIComponent* CustomerAI);

  //* Store
  UPROPERTY(EditAnywhere, Category = "Character | Store")
  class AStore* Store;

  //* Game Store Phase Manager - to control the global game state.
  UPROPERTY(EditAnywhere, Category = "Character | StorePhaseManager")
  class AStorePhaseManager* StorePhaseManager;
};
