// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Item/ItemBase.h"

UInteractionComponent::UInteractionComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  InteractionType = EInteractionType::Use;
}

void UInteractionComponent::BeginPlay() { Super::BeginPlay(); }

void UInteractionComponent::InteractUse(FUIOnInteract* UIOnInteract) const {}

std::tuple<UItemBase*, UCustomerAIComponent*> UInteractionComponent::InteractNpc(FUIOnInteract* UIOnInteract) const {
  UInventoryComponent* OwnerInventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
  if (!OwnerInventory) return {nullptr, nullptr};

  UCustomerAIComponent* OwnerCustomerAI = GetOwner()->FindComponentByClass<UCustomerAIComponent>();
  if (!OwnerCustomerAI) return {nullptr, nullptr};

  if (OwnerInventory->ItemsArray.Num() > 0) return {OwnerInventory->ItemsArray[0], OwnerCustomerAI};

  return {nullptr, nullptr};
}

std::tuple<class UInventoryComponent*, int16> UInteractionComponent::InteractStore() const {
  UInventoryComponent* OwnerInventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
  if (!OwnerInventory) return {nullptr, 0};

  int16 StoreMoney = 1000;
  return {OwnerInventory, StoreMoney};
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