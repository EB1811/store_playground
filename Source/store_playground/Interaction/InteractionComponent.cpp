// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Store/StockDisplayComponent.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/AI/NegotiationAI.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Market/NpcStoreComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/WorldObject/Buildable.h"
#include "store_playground/Level/LevelChangeComponent.h"
#include "store_playground/Quest/QuestComponent.h"
#include "store_playground/Upgrade/UpgradeSelectComponent.h"
#include "store_playground/Minigame/MiniGameComponent.h"

UInteractionComponent::UInteractionComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  InteractionType = EInteractionType::Use;
}

void UInteractionComponent::BeginPlay() { Super::BeginPlay(); }

void UInteractionComponent::InteractUse() const {}

auto UInteractionComponent::InteractLevelChange() const -> ULevelChangeComponent* {
  ULevelChangeComponent* OwnerLevelChangeC = GetOwner()->FindComponentByClass<ULevelChangeComponent>();
  check(OwnerLevelChangeC);

  return OwnerLevelChangeC;
}

auto UInteractionComponent::InteractUpgradeSelect() const -> class UUpgradeSelectComponent* {
  UUpgradeSelectComponent* OwnerUpgradeSelectC = GetOwner()->FindComponentByClass<UUpgradeSelectComponent>();
  check(OwnerUpgradeSelectC);

  return OwnerUpgradeSelectC;
}

auto UInteractionComponent::InteractBuildable() const -> TOptional<class ABuildable*> {
  ABuildable* OwnerBuildable = Cast<ABuildable>(GetOwner());
  check(OwnerBuildable);

  return OwnerBuildable;
}

auto UInteractionComponent::InteractStockDisplay() const
    -> TTuple<class UStockDisplayComponent*, class UInventoryComponent*> {
  UStockDisplayComponent* OwnerStockDisplayC = GetOwner()->FindComponentByClass<UStockDisplayComponent>();
  UInventoryComponent* OwnerInventoryC = GetOwner()->FindComponentByClass<UInventoryComponent>();
  check(OwnerStockDisplayC);
  check(OwnerInventoryC);

  return {OwnerStockDisplayC, OwnerInventoryC};
}

auto UInteractionComponent::InteractNPCDialogue() const -> TOptional<TArray<FDialogueData>> {
  UDialogueComponent* OwnerDialogueC = GetOwner()->FindComponentByClass<UDialogueComponent>();
  check(OwnerDialogueC);

  return OwnerDialogueC->DialogueArray;
}

auto UInteractionComponent::InteractWaitingCustomer() const -> TTuple<class UCustomerAIComponent*, class UItemBase*> {
  UCustomerAIComponent* OwnerCustomerAIC = GetOwner()->FindComponentByClass<UCustomerAIComponent>();
  check(OwnerCustomerAIC);

  return {OwnerCustomerAIC, OwnerCustomerAIC->NegotiationAI->RelevantItem};
}

auto UInteractionComponent::InteractUniqueNPCQuest() const
    -> TTuple<class UDialogueComponent*, class UQuestComponent*, class UCustomerAIComponent*, class UItemBase*> {
  UCustomerAIComponent* OwnerCustomerAIC = GetOwner()->FindComponentByClass<UCustomerAIComponent>();
  UDialogueComponent* OwnerDialogueC = GetOwner()->FindComponentByClass<UDialogueComponent>();
  UQuestComponent* OwnerQuestC = GetOwner()->FindComponentByClass<UQuestComponent>();
  check(OwnerDialogueC);
  check(OwnerQuestC);

  class UItemBase* RelevantItem = OwnerCustomerAIC ? OwnerCustomerAIC->NegotiationAI->RelevantItem : nullptr;

  return {OwnerDialogueC, OwnerQuestC, OwnerCustomerAIC, RelevantItem};
}

auto UInteractionComponent::InteractNpcStore() const
    -> TTuple<class UNpcStoreComponent*, class UInventoryComponent*, class UDialogueComponent*> {
  UNpcStoreComponent* OwnerNpcStoreC = GetOwner()->FindComponentByClass<UNpcStoreComponent>();
  UInventoryComponent* OwnerInventoryC = GetOwner()->FindComponentByClass<UInventoryComponent>();
  UDialogueComponent* OwnerDialogueC = GetOwner()->FindComponentByClass<UDialogueComponent>();
  check(OwnerNpcStoreC);
  check(OwnerInventoryC);
  check(OwnerDialogueC);

  return {OwnerNpcStoreC, OwnerInventoryC, OwnerDialogueC};
}

auto UInteractionComponent::InteractContainer() const -> class UInventoryComponent* {
  UInventoryComponent* OwnerInventoryC = GetOwner()->FindComponentByClass<UInventoryComponent>();
  check(OwnerInventoryC);

  return OwnerInventoryC;
}

auto UInteractionComponent::InteractMiniGame() const -> TTuple<class UMiniGameComponent*, class UDialogueComponent*> {
  UMiniGameComponent* OwnerMiniGameC = GetOwner()->FindComponentByClass<UMiniGameComponent>();
  UDialogueComponent* OwnerDialogueC = GetOwner()->FindComponentByClass<UDialogueComponent>();
  check(OwnerMiniGameC);
  check(OwnerDialogueC);

  return {OwnerMiniGameC, OwnerDialogueC};
}