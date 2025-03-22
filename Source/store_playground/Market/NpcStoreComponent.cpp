#include "NpcStoreComponent.h"

UNpcStoreComponent::UNpcStoreComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  NpcStoreID = FGuid::NewGuid();
}

void UNpcStoreComponent::BeginPlay() { Super::BeginPlay(); }