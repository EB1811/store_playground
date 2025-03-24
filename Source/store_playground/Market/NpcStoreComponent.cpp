#include "NpcStoreComponent.h"

UNpcStoreComponent::UNpcStoreComponent() {
  PrimaryComponentTick.bCanEverTick = false;

  NpcStoreCID = FGuid::NewGuid();
}

void UNpcStoreComponent::BeginPlay() { Super::BeginPlay(); }