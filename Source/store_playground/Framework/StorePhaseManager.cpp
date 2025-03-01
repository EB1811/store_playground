
#include "StorePhaseManager.h"
#include "store_playground/AI/CustomerAIManager.h"

EShopPhaseState GetNextShopPhaseState(EShopPhaseState CurrentState, EShopPhaseAction Action) {
  switch (CurrentState) {
    case EShopPhaseState::None:
      if (Action == EShopPhaseAction::Start) return EShopPhaseState::Morning;

    case EShopPhaseState::Morning:
      if (Action == EShopPhaseAction::OpenShop) return EShopPhaseState::ShopOpen;
    case EShopPhaseState::ShopOpen:
      if (Action == EShopPhaseAction::CloseShop) return EShopPhaseState::Night;
    case EShopPhaseState::Night:
      if (Action == EShopPhaseAction::EndDay) return EShopPhaseState::Morning;
    default: break;
  }

  checkf(false, TEXT("Invalid state transition, CurrentState: %d, Action: %d"), CurrentState, Action);
  return EShopPhaseState::None;
}

AStorePhaseManager::AStorePhaseManager() {
  PrimaryActorTick.bCanEverTick = false;

  ShopPhaseState = EShopPhaseState::None;
}

void AStorePhaseManager::BeginPlay() { Super::BeginPlay(); }

void AStorePhaseManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AStorePhaseManager::Start() {
  check(Store && Market && CustomerAIManager);
  ShopPhaseState = GetNextShopPhaseState(ShopPhaseState, EShopPhaseAction::Start);
}

void AStorePhaseManager::OpenShop() {
  ShopPhaseState = GetNextShopPhaseState(ShopPhaseState, EShopPhaseAction::OpenShop);

  CustomerAIManager->StartCustomerAI();
}

void AStorePhaseManager::CloseShop() {
  ShopPhaseState = GetNextShopPhaseState(ShopPhaseState, EShopPhaseAction::CloseShop);

  CustomerAIManager->EndCustomerAI();
}

void AStorePhaseManager::EndDay() { ShopPhaseState = GetNextShopPhaseState(ShopPhaseState, EShopPhaseAction::EndDay); }

void AStorePhaseManager::NextPhase() {
  UE_LOG(LogTemp, Warning, TEXT("Next phase."));

  switch (ShopPhaseState) {
    case EShopPhaseState::Morning: OpenShop(); break;
    case EShopPhaseState::ShopOpen: CloseShop(); break;
    case EShopPhaseState::Night: EndDay(); break;
    default: break;
  }
}