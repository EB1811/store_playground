// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <optional>
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "InteractionComponent.generated.h"

// temp: move to player ui controller
DECLARE_DELEGATE_OneParam(FUIOnInteract, int32);

UENUM()
enum class EInteractionType : uint8 {
  None UMETA(DisplayName = "None"),
  // ? Or just have it on the hud?
  StoreNextPhase UMETA(DisplayName = "Store Next Phase"),
  Stock UMETA(DisplayName = "Stock"),
  Use UMETA(DisplayName = "Use"),
  // ? Combine and use customer state?
  NPCDialogue UMETA(DisplayName = "Npc Dialogue"),
  WaitingCustomer UMETA(DisplayName = "Waiting Customer"),
  WaitingUniqueCustomer UMETA(DisplayName = "Waiting Unique Customer"),
  Store UMETA(DisplayName = "Store"),
  Container UMETA(DisplayName = "Container"),
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UInteractionComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UInteractionComponent();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Interaction")
  EInteractionType InteractionType;

  void InteractUse(FUIOnInteract* UIOnInteract = nullptr) const;
  class UInventoryComponent* InteractStock() const;

  std::optional<TArray<FDialogueData>> InteractNPCDialogue() const;
  std::tuple<const class UItemBase*, class UCustomerAIComponent*> InteractWaitingCustomer() const;
  std::tuple<const class UItemBase*, class UCustomerAIComponent*, class UDialogueComponent*>
  InteractWaitingUniqueCustomer() const;

  std::tuple<class UInventoryComponent*, class UDialogueComponent*> InteractStore() const;
  class UInventoryComponent* InteractContainer() const;
};

// ? Should there be different components for different types of interactions?