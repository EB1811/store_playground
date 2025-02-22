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
  Use UMETA(DisplayName = "Use"),
  NPCDialogue UMETA(DisplayName = "Npc Dialogue"),
  WaitingCustomer UMETA(DisplayName = "Waiting Customer"),
  Store UMETA(DisplayName = "Store"),
  Container UMETA(DisplayName = "Container"),
  Stock UMETA(DisplayName = "Stock")
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
  std::optional<TArray<FDialogueData>> InteractNPCDialogue() const;
  std::tuple<class UItemBase*, class UCustomerAIComponent*> InteractWaitingCustomer() const;
  // TODO: Return market data.
  std::tuple<class UInventoryComponent*, int32> InteractStore() const;
  class UInventoryComponent* InteractContainer() const;
  class UInventoryComponent* InteractStock() const;
};

// ? Should there be different components for different types of interactions?