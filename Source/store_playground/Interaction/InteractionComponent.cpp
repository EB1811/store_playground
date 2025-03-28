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

UInteractionComponent::UInteractionComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  InteractionType = EInteractionType::Use;
}

void UInteractionComponent::BeginPlay() { Super::BeginPlay(); }

void UInteractionComponent::InteractUse(FUIOnInteract* UIOnInteract) const {}

ULevelChangeComponent* UInteractionComponent::InteractLevelChange() const {
  ULevelChangeComponent* OwnerLevelChangeC = GetOwner()->FindComponentByClass<ULevelChangeComponent>();
  check(OwnerLevelChangeC);

  return OwnerLevelChangeC;
}

TOptional<class ABuildable*> UInteractionComponent::InteractBuildable() const {
  ABuildable* OwnerBuildable = Cast<ABuildable>(GetOwner());
  check(OwnerBuildable);

  return OwnerBuildable;
}

TTuple<UStockDisplayComponent*, UInventoryComponent*> UInteractionComponent::InteractStockDisplay() const {
  UStockDisplayComponent* OwnerStockDisplayC = GetOwner()->FindComponentByClass<UStockDisplayComponent>();
  UInventoryComponent* OwnerInventoryC = GetOwner()->FindComponentByClass<UInventoryComponent>();
  check(OwnerStockDisplayC);
  check(OwnerInventoryC);

  return {OwnerStockDisplayC, OwnerInventoryC};
}

TOptional<TArray<struct FDialogueData>> UInteractionComponent::InteractNPCDialogue() const {
  UDialogueComponent* OwnerDialogueC = GetOwner()->FindComponentByClass<UDialogueComponent>();
  check(OwnerDialogueC);

  return OwnerDialogueC->DialogueArray;
}

TTuple<class UCustomerAIComponent*, const class UItemBase*> UInteractionComponent::InteractWaitingCustomer() const {
  UCustomerAIComponent* OwnerCustomerAIC = GetOwner()->FindComponentByClass<UCustomerAIComponent>();
  check(OwnerCustomerAIC);

  return {OwnerCustomerAIC, OwnerCustomerAIC->NegotiationAI->RelevantItem};
}

TTuple<UDialogueComponent*, UQuestComponent*, UCustomerAIComponent*, const UItemBase*>
UInteractionComponent::InteractUniqueNPCQuest() const {
  UCustomerAIComponent* OwnerCustomerAIC = GetOwner()->FindComponentByClass<UCustomerAIComponent>();
  UDialogueComponent* OwnerDialogueC = GetOwner()->FindComponentByClass<UDialogueComponent>();
  UQuestComponent* OwnerQuestC = GetOwner()->FindComponentByClass<UQuestComponent>();
  check(OwnerDialogueC);
  check(OwnerQuestC);

  const class UItemBase* RelevantItem = OwnerCustomerAIC ? OwnerCustomerAIC->NegotiationAI->RelevantItem : nullptr;

  return {OwnerDialogueC, OwnerQuestC, OwnerCustomerAIC, RelevantItem};
}

TTuple<UNpcStoreComponent*, UInventoryComponent*, UDialogueComponent*> UInteractionComponent::InteractNpcStore() const {
  UNpcStoreComponent* OwnerNpcStoreC = GetOwner()->FindComponentByClass<UNpcStoreComponent>();
  UInventoryComponent* OwnerInventoryC = GetOwner()->FindComponentByClass<UInventoryComponent>();
  UDialogueComponent* OwnerDialogueC = GetOwner()->FindComponentByClass<UDialogueComponent>();
  check(OwnerNpcStoreC);
  check(OwnerInventoryC);
  check(OwnerDialogueC);

  return {OwnerNpcStoreC, OwnerInventoryC, OwnerDialogueC};
}

UInventoryComponent* UInteractionComponent::InteractContainer() const {
  UInventoryComponent* OwnerInventoryC = GetOwner()->FindComponentByClass<UInventoryComponent>();
  check(OwnerInventoryC);

  return OwnerInventoryC;
}