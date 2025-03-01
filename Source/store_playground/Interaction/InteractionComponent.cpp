// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/AI/NegotiationAI.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/Item/ItemBase.h"

UInteractionComponent::UInteractionComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  InteractionType = EInteractionType::Use;
}

void UInteractionComponent::BeginPlay() { Super::BeginPlay(); }

void UInteractionComponent::InteractUse(FUIOnInteract* UIOnInteract) const {}

std::optional<TArray<struct FDialogueData>> UInteractionComponent::InteractNPCDialogue() const {
  UDialogueComponent* OwnerDialogue = GetOwner()->FindComponentByClass<UDialogueComponent>();
  if (!OwnerDialogue) return std::nullopt;

  return OwnerDialogue->DialogueArray;
}

std::tuple<const class UItemBase*, class UCustomerAIComponent*> UInteractionComponent::InteractWaitingCustomer() const {
  UCustomerAIComponent* OwnerCustomerAI = GetOwner()->FindComponentByClass<UCustomerAIComponent>();
  if (!OwnerCustomerAI) return {nullptr, nullptr};

  check(OwnerCustomerAI->NegotiationAI->WantedItem);
  return {OwnerCustomerAI->NegotiationAI->WantedItem, OwnerCustomerAI};
}

std::tuple<const class UItemBase*, UCustomerAIComponent*, UDialogueComponent*>
UInteractionComponent::InteractWaitingUniqueCustomer() const {
  UCustomerAIComponent* OwnerCustomerAI = GetOwner()->FindComponentByClass<UCustomerAIComponent>();
  if (!OwnerCustomerAI) return {nullptr, nullptr, nullptr};

  check(OwnerCustomerAI->NegotiationAI->WantedItem);

  UDialogueComponent* OwnerDialogue = GetOwner()->FindComponentByClass<UDialogueComponent>();
  if (!OwnerDialogue) return {OwnerCustomerAI->NegotiationAI->WantedItem, OwnerCustomerAI, nullptr};

  return {OwnerCustomerAI->NegotiationAI->WantedItem, OwnerCustomerAI, OwnerDialogue};
}

std::tuple<UInventoryComponent*, UDialogueComponent*> UInteractionComponent::InteractStore() const {
  UInventoryComponent* OwnerInventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
  if (!OwnerInventory) return {nullptr, nullptr};

  UDialogueComponent* OwnerDialogue = GetOwner()->FindComponentByClass<UDialogueComponent>();
  if (!OwnerDialogue) return {OwnerInventory, nullptr};

  return {OwnerInventory, OwnerDialogue};
}

UInventoryComponent* UInteractionComponent::InteractContainer() const {
  UInventoryComponent* OwnerInventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
  if (!OwnerInventory) return nullptr;

  return OwnerInventory;
}

UInventoryComponent* UInteractionComponent::InteractStock() const {
  UInventoryComponent* OwnerInventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
  if (!OwnerInventory) return nullptr;

  return OwnerInventory;
}