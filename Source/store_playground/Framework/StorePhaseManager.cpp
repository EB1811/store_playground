
#include "StorePhaseManager.h"
#include "Engine/World.h"
#include "Misc/AssertionMacros.h"
#include "TimerManager.h"
#include "store_playground/AI/CustomerAIManager.h"
#include "store_playground/WorldObject/Buildable.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Store/Store.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/Player/PlayerCommand.h"
#include "store_playground/Sound/MusicManager.h"
#include "store_playground/UI/SpgHUD.h"
#include "Kismet/GameplayStatics.h"
#include "store_playground/Upgrade/UpgradeManager.h"

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

  BehaviorParams = {
      .OpenShopDuration = 60.0f,
  };

  StorePhaseState = EStorePhaseState::None;

  Upgradeable.ChangeBehaviorParam = [this](const TMap<FName, float>& ParamValues) { ChangeBehaviorParam(ParamValues); };
}

void AStorePhaseManager::BeginPlay() {
  Super::BeginPlay();
  check(BuildableClass);
}

void AStorePhaseManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AStorePhaseManager::OnOpenShopTimerEnd() {
  if (StorePhaseState != EStorePhaseState::ShopOpen) return;

  PlayerCommand->CommandExitCurrentAction();
  NextPhase();

  UE_LOG(LogTemp, Log, TEXT("Open shop timer ended."));
}

void AStorePhaseManager::ShopOpenConsiderPlayerState(EPlayerState PlayerBehaviourState) {
  check(StorePhaseState == EStorePhaseState::ShopOpen);

  switch (PlayerBehaviourState) {
    case EPlayerState::FocussedMenu:
    case EPlayerState::Cutscene: {
      GetWorld()->GetTimerManager().PauseTimer(OpenShopTimerHandle);
      break;
    }
    case EPlayerState::Normal: {
      GetWorld()->GetTimerManager().UnPauseTimer(OpenShopTimerHandle);
      break;
    }
  }
}
void AStorePhaseManager::Start() {
  check(CustomerAIManager);

  if (StorePhaseState == EStorePhaseState::None) {
    StorePhaseState = GetNextStorePhaseState(StorePhaseState, EStorePhaseAction::Start);

    MusicManager->MorningMusicCalled();
    Store->SetupStoreEnvironment();
    Store->EnterLevel();

    DayManager->StartNewDay();
    return;
  }

  Store->SetupStoreEnvironment();
  switch (StorePhaseState) {
    case EStorePhaseState::Morning: MusicManager->MorningMusicCalled(); break;
    case EStorePhaseState::ShopOpen: MusicManager->ShopOpenMusicCalled(); break;
    case EStorePhaseState::Night: MusicManager->NightMusicCalled(); break;
    default: checkNoEntry();
  }
}

void AStorePhaseManager::ToggleBuildMode() {
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

  MusicManager->ShopOpenMusicCalled();
  Store->SetupStoreEnvironment();

  GetWorld()->GetTimerManager().SetTimer(OpenShopTimerHandle, this, &AStorePhaseManager::OnOpenShopTimerEnd,
                                         BehaviorParams.OpenShopDuration, false);
  CustomerAIManager->StartCustomerAI();
}

void AStorePhaseManager::CloseShop() {
  StorePhaseState = GetNextStorePhaseState(StorePhaseState, EStorePhaseAction::CloseShop);

  MusicManager->NightMusicCalled();
  Store->SetupStoreEnvironment();

  GetWorld()->GetTimerManager().ClearTimer(OpenShopTimerHandle);
  CustomerAIManager->EndCustomerAI();
}

void AStorePhaseManager::EndDay() {
  StorePhaseState = GetNextStorePhaseState(StorePhaseState, EStorePhaseAction::EndDay);

  MusicManager->MorningMusicCalled();
  Store->SetupStoreEnvironment();

  DayManager->StartNewDay();
  UpgradeManager->ConsiderUpgradePoints();

  SaveManager->AutoSave();
}

void AStorePhaseManager::NextPhase() {
  ASpgHUD* HUD = Cast<ASpgHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());  // ? Save as reference?
  HUD->StorePhaseTransition([this]() {
    switch (StorePhaseState) {
      case EStorePhaseState::Morning: OpenShop(); break;
      case EStorePhaseState::MorningBuildMode:
        ToggleBuildMode();
        OpenShop();
        break;
      case EStorePhaseState::ShopOpen: CloseShop(); break;
      case EStorePhaseState::Night: EndDay(); break;
      default: break;
    }

    UE_LOG(LogTemp, Log, TEXT("Next phase: %s"), *UEnum::GetValueAsString(StorePhaseState));
  });

  if (StorePhaseState != EStorePhaseState::MorningBuildMode) UGameplayStatics::PlaySound2D(this, NextPhaseSound, 1.0f);
}

void AStorePhaseManager::EnterBuildMode() {
  if (StorePhaseState != EStorePhaseState::Morning && StorePhaseState != EStorePhaseState::MorningBuildMode) return;

  ASpgHUD* HUD = Cast<ASpgHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());  // ? Save as reference?
  HUD->StorePhaseTransition([this]() {
    ToggleBuildMode();

    PlayerCommand->ResetPosition();
  });
}

void AStorePhaseManager::ChangeBehaviorParam(const TMap<FName, float>& ParamValues) {
  for (const auto& ParamPair : ParamValues) {
    auto StructProp = CastField<FStructProperty>(this->GetClass()->FindPropertyByName("BehaviorParams"));
    auto StructPtr = StructProp->ContainerPtrToValuePtr<void>(this);
    AddToStructPropertyValue(StructProp, StructPtr, ParamPair.Key, ParamPair.Value);
  }
}