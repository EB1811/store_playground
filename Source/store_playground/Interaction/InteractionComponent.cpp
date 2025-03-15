// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/Store/StockDisplayComponent.h"
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

std::tuple<UStockDisplayComponent*, UInventoryComponent*> UInteractionComponent::InteractStockDisplay() const {
  UStockDisplayComponent* OwnerStockDisplayC = GetOwner()->FindComponentByClass<UStockDisplayComponent>();
  UInventoryComponent* OwnerInventoryC = GetOwner()->FindComponentByClass<UInventoryComponent>();

  check(OwnerStockDisplayC);
  check(OwnerInventoryC);

  return {OwnerStockDisplayC, OwnerInventoryC};
}

std::optional<TArray<struct FDialogueData>> UInteractionComponent::InteractNPCDialogue() const {
  UDialogueComponent* OwnerDialogueC = GetOwner()->FindComponentByClass<UDialogueComponent>();

  check(OwnerDialogueC);

  return OwnerDialogueC->DialogueArray;
}

std::tuple<const class UItemBase*, class UCustomerAIComponent*> UInteractionComponent::InteractWaitingCustomer() const {
  UCustomerAIComponent* OwnerCustomerAIC = GetOwner()->FindComponentByClass<UCustomerAIComponent>();
  check(OwnerCustomerAIC);

  return {OwnerCustomerAIC->NegotiationAI->RelevantItem, OwnerCustomerAIC};
}

std::tuple<const class UItemBase*, UCustomerAIComponent*, UDialogueComponent*>
UInteractionComponent::InteractWaitingUniqueCustomer() const {
  UCustomerAIComponent* OwnerCustomerAIC = GetOwner()->FindComponentByClass<UCustomerAIComponent>();
  UDialogueComponent* OwnerDialogueC = GetOwner()->FindComponentByClass<UDialogueComponent>();

  check(OwnerCustomerAIC);
  check(OwnerCustomerAIC->NegotiationAI->RelevantItem);
  check(OwnerDialogueC);

  return {OwnerCustomerAIC->NegotiationAI->RelevantItem, OwnerCustomerAIC, OwnerDialogueC};
}

std::tuple<UInventoryComponent*, UDialogueComponent*> UInteractionComponent::InteractNpcStore() const {
  UInventoryComponent* OwnerInventoryC = GetOwner()->FindComponentByClass<UInventoryComponent>();
  UDialogueComponent* OwnerDialogueC = GetOwner()->FindComponentByClass<UDialogueComponent>();

  check(OwnerInventoryC);
  check(OwnerDialogueC);

  return {OwnerInventoryC, OwnerDialogueC};
}

UInventoryComponent* UInteractionComponent::InteractContainer() const {
  UInventoryComponent* OwnerInventoryC = GetOwner()->FindComponentByClass<UInventoryComponent>();

  check(OwnerInventoryC);

  return OwnerInventoryC;
}