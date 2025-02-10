// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionComponent.h"
#include "store_playground/Inventory/InventoryComponent.h"

void UInteractionComponent::BeginPlay() { Super::BeginPlay(); }

void UInteractionComponent::InteractUse(FUIOnInteract* UIOnInteract) const {
  if (UIOnInteract) UIOnInteract->ExecuteIfBound(0);
}

void UInteractionComponent::InteractNpc(FUIOnInteract* UIOnInteract) const {
  if (UIOnInteract) UIOnInteract->ExecuteIfBound(0);
}

void UInteractionComponent::InteractPickup(FUIOnInteract* UIOnInteract) const {
  if (UIOnInteract) UIOnInteract->ExecuteIfBound(0);
}

UInventoryComponent* UInteractionComponent::InteractContainer() const {
  UInventoryComponent* OwnerInventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
  if (!OwnerInventory) return nullptr;

  UE_LOG(LogTemp, Warning, TEXT("InteractContainer"));

  return OwnerInventory;
}