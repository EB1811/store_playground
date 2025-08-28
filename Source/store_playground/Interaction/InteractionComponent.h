// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <optional>
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "InteractionComponent.generated.h"

// ? Add start interaction and end interaction events/callbacks.

UENUM()
enum class EInteractionType : uint8 {
  None UMETA(DisplayName = "None"),
  Use UMETA(DisplayName = "Use"),

  LevelChange UMETA(DisplayName = "Level Change"),
  LeaveStore UMETA(DisplayName = "Leave Store"),
  StoreNextPhase UMETA(DisplayName = "Store Next Phase"),

  UpgradeSelect UMETA(DisplayName = "Upgrade Select"),
  AbilitySelect UMETA(DisplayName = "Ability Select"),

  Buildable UMETA(DisplayName = "Buildable"),
  StockDisplay UMETA(DisplayName = "StockDisplay"),
  Decoration UMETA(DisplayName = "Decoration"),

  NPCDialogue UMETA(DisplayName = "Npc Dialogue"),
  Customer UMETA(DisplayName = "Customer"),
  WaitingCustomer UMETA(DisplayName = "Waiting Customer"),
  UniqueNPCQuest UMETA(DisplayName = "Unique Npc Quest"),

  NpcStore UMETA(DisplayName = "NPC Store"),
  Container UMETA(DisplayName = "Container"),

  Pickup UMETA(DisplayName = "Pickup"),
  MiniGame UMETA(DisplayName = "MiniGame"),
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UInteractionComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UInteractionComponent();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Interaction", SaveGame)
  EInteractionType InteractionType;

  UPROPERTY(EditAnywhere)
  class USoundBase* InteractionSound;

  void PlayInteractionSound() const;

  UPROPERTY(EditAnywhere, Category = "Interaction")
  bool bIsInteracting;  // * For interactions that need tracking.
  void StartInteraction();
  void EndInteraction();

  void InteractUse() const;

  auto InteractLevelChange() const -> class ULevelChangeComponent*;

  auto InteractUpgradeSelect() const -> class UUpgradeSelectComponent*;
  auto InteractBuildable() const -> TOptional<class ABuildable*>;
  auto InteractStockDisplay() const -> TTuple<class UStockDisplayComponent*, class UInventoryComponent*>;

  auto InteractNPCDialogue() const -> TTuple<class UDialogueComponent*, class USimpleSpriteAnimComponent*>;
  auto InteractCustomer() const
      -> TTuple<class UDialogueComponent*, class UCustomerAIComponent*, class USimpleSpriteAnimComponent*>;
  auto InteractWaitingCustomer() const
      -> TTuple<class UCustomerAIComponent*, class UItemBase*, class USimpleSpriteAnimComponent*>;
  auto InteractUniqueNPCQuest() const -> TTuple<class UDialogueComponent*,
                                                class UQuestComponent*,
                                                class UCustomerAIComponent*,
                                                class UItemBase*,
                                                class USimpleSpriteAnimComponent*>;

  auto InteractNpcStore() const
      -> TTuple<class UNpcStoreComponent*, class UInventoryComponent*, class UDialogueComponent*>;
  auto InteractContainer() const -> class UInventoryComponent*;

  auto InteractPickup() const -> TOptional<class UPickupComponent*>;
  auto InteractMiniGame() const -> TTuple<class UMiniGameComponent*, class UDialogueComponent*>;
};
