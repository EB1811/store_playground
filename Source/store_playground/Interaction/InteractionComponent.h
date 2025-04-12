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

  LevelChange UMETA(DisplayName = "Level Change"),
  StoreNextPhase UMETA(DisplayName = "Store Next Phase"),

  UpgradeSelect UMETA(DisplayName = "Upgrade Select"),

  Buildable UMETA(DisplayName = "Buildable"),
  StockDisplay UMETA(DisplayName = "StockDisplay"),
  Decoration UMETA(DisplayName = "Decoration"),

  Use UMETA(DisplayName = "Use"),
  NPCDialogue UMETA(DisplayName = "Npc Dialogue"),
  WaitingCustomer UMETA(DisplayName = "Waiting Customer"),
  UniqueNPCQuest UMETA(DisplayName = "Unique Npc Quest"),
  NpcStore UMETA(DisplayName = "NPC Store"),
  Container UMETA(DisplayName = "Container"),
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UInteractionComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UInteractionComponent();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Interaction", SaveGame)
  EInteractionType InteractionType;

  void InteractUse(FUIOnInteract* UIOnInteract = nullptr) const;

  auto InteractLevelChange() const -> class ULevelChangeComponent*;

  auto InteractUpgradeSelect() const -> class UUpgradeSelectComponent*;

  auto InteractBuildable() const -> TOptional<class ABuildable*>;
  auto InteractStockDisplay() const -> TTuple<class UStockDisplayComponent*, class UInventoryComponent*>;

  auto InteractNPCDialogue() const -> TOptional<TArray<FDialogueData>>;
  auto InteractWaitingCustomer() const -> TTuple<class UCustomerAIComponent*, const class UItemBase*>;

  auto InteractUniqueNPCQuest() const
      -> TTuple<class UDialogueComponent*, class UQuestComponent*, class UCustomerAIComponent*, const class UItemBase*>;
  auto InteractNpcStore() const
      -> TTuple<class UNpcStoreComponent*, class UInventoryComponent*, class UDialogueComponent*>;
  auto InteractContainer() const -> class UInventoryComponent*;
};
