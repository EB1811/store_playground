
#include "StorePhaseManager.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/WorldObject/Buildable.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/Lighting/StorePhaseLightingManager.h"
#include "store_playground/Player/PlayerCommand.h"
#include "store_playground/Sound/MusicManager.h"
#include "store_playground/UI/SpgHUD.h"

EStorePhaseState GetNextStorePhaseState(EStorePhaseState CurrentState, EStorePhaseAction Action) {
  switch (CurrentState) {
    case EStorePhaseState::None:
      if (Action == EStorePhaseAction::Start) return EStorePhaseState::Morning;
    case EStorePhaseState::Morning:
      if (Action == EStorePhaseAction::ToggleBuildMode) return EStorePhaseState::MorningBuildMode;
      if (Action == EStorePhaseAction::OpenShop) return EStorePhaseState::ShopOpen;
    case EStorePhaseState::MorningBuildMode:
      if (Action == EStorePhaseAction::ToggleBuildMode) return EStorePhaseState::Morning;
      if (Action == EStorePhaseAction::OpenShop) return EStorePhaseState::ShopOpen;
    case EStorePhaseState::ShopOpen:
      if (Action == EStorePhaseAction::CloseShop) return EStorePhaseState::Night;
    case EStorePhaseState::Night:
      if (Action == EStorePhaseAction::EndDay) return EStorePhaseState::Morning;
    default: break;
  }

  checkf(false, TEXT("Invalid state transition, CurrentState: %d, Action: %d"), CurrentState, Action);
  return EStorePhaseState::None;
}

AStorePhaseManager::AStorePhaseManager() {
  PrimaryActorTick.bCanEverTick = false;

  StorePhaseState = EStorePhaseState::None;
}

void AStorePhaseManager::BeginPlay() {
  Super::BeginPlay();
  check(BuildableClass);
}

void AStorePhaseManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AStorePhaseManager::Start() {
  check(CustomerAIManager);
  StorePhaseState = GetNextStorePhaseState(StorePhaseState, EStorePhaseAction::Start);

  DayManager->StartNewDay();
}

void AStorePhaseManager::BuildMode() {
  StorePhaseState = GetNextStorePhaseState(StorePhaseState, EStorePhaseAction::ToggleBuildMode);

  TArray<ABuildable*> EmptyBuildables =
      GetAllActorsOf<ABuildable>(GetWorld(), BuildableClass).FilterByPredicate([](ABuildable* Buildable) {
        return Buildable->BuildableType == EBuildableType::None;
      });

  for (ABuildable* Buildable : EmptyBuildables) {
    switch (StorePhaseState) {
      case EStorePhaseState::MorningBuildMode:
        Buildable->Mesh->SetVisibility(true);
        Buildable->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        break;
      case EStorePhaseState::Morning:
        Buildable->Mesh->SetVisibility(false);
        Buildable->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        break;
    }
  }
}

void AStorePhaseManager::OpenShop() {
  StorePhaseState = GetNextStorePhaseState(StorePhaseState, EStorePhaseAction::OpenShop);

  StorePhaseLightingManager->OnOpenShopLightingCalled();
  // MusicManager->OnOpenShopMusicCalled();

  GetWorld()->GetTimerManager().SetTimer(OpenShopTimerHandle, this, &AStorePhaseManager::OnOpenShopTimerEnd,
                                         StorePhaseManagerParams.OpenShopDuration, false);
  CustomerAIManager->StartCustomerAI();
}

void AStorePhaseManager::CloseShop() {
  StorePhaseState = GetNextStorePhaseState(StorePhaseState, EStorePhaseAction::CloseShop);

  StorePhaseLightingManager->OnCloseShopLightingCalled();
  // MusicManager->OnCloseShopMusicCalled();

  GetWorld()->GetTimerManager().ClearTimer(OpenShopTimerHandle);
  CustomerAIManager->EndCustomerAI();
}

void AStorePhaseManager::EndDay() {
  StorePhaseState = GetNextStorePhaseState(StorePhaseState, EStorePhaseAction::EndDay);

  StorePhaseLightingManager->OnEndDayLightingCalled();
  // MusicManager->OnEndDayMusicCalled();

  DayManager->StartNewDay();
  // SaveManager->CreateNewSaveGame();
}

void AStorePhaseManager::NextPhase() {
  ASpgHUD* HUD = Cast<ASpgHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
  HUD->StorePhaseTransition([this]() {
    switch (StorePhaseState) {
      case EStorePhaseState::Morning: OpenShop(); break;
      case EStorePhaseState::MorningBuildMode:
        BuildMode();
        OpenShop();
        break;
      case EStorePhaseState::ShopOpen: CloseShop(); break;
      case EStorePhaseState::Night: EndDay(); break;
      default: break;
    }

    UE_LOG(LogTemp, Warning, TEXT("Next phase: %s"), *UEnum::GetDisplayValueAsText(StorePhaseState).ToString());
  });
}

void AStorePhaseManager::OnOpenShopTimerEnd() {
  if (StorePhaseState != EStorePhaseState::ShopOpen) return;

  PlayerCommand->CommandExitCurrentAction();
  NextPhase();

  UE_LOG(LogTemp, Warning, TEXT("Open shop timer ended."));
}