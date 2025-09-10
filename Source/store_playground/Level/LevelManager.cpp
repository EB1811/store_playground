#include "LevelManager.h"
#include "Containers/ContainersFwd.h"
#include "Engine/LevelStreaming.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "UObject/Class.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Level/LevelStructs.h"
#include "store_playground/Lighting/StorePhaseLightingManager.h"
#include "store_playground/Player/PlayerCommand.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Level/MarketLevel.h"
#include "store_playground/Level/ChurchLevel.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"
#include "store_playground/UI/SpgHUD.h"
#include "store_playground/Cutscene/CutsceneManager.h"

ALevelManager::ALevelManager() { PrimaryActorTick.bCanEverTick = false; }

void ALevelManager::BeginPlay() {
  Super::BeginPlay();

  for (ELevel Level : TEnumRange<ELevel>()) check(LevelNames.Contains(Level));

  HUD = Cast<ASpgHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
  check(HUD);
}

void ALevelManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ALevelManager::InitLoadStore(std::function<void()> _LevelReadyFunc) {
  LevelNames[ELevel::Store] = StoreExpansionLevelMap[StoreExpansionManager->CurrentStoreExpansionLevelID];

  if (ULevelStreaming* Streaming = UGameplayStatics::GetStreamingLevel(this, LevelNames[ELevel::Store])) {
    LoadedLevel = ELevel::Store;
    LevelReadyFunc = _LevelReadyFunc;

    Streaming->OnLevelShown.Clear();
    Streaming->OnLevelShown.AddDynamic(this, &ALevelManager::OnLevelShown);

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.UUID = 1;
    UGameplayStatics::LoadStreamLevel(this, LevelNames[ELevel::Store], true, true, LatentInfo);
    // GetWorld()->FlushLevelStreaming();  // ! Breaks component begin play and tick.
  }
}

void ALevelManager::BeginLoadLevel(ELevel Level, std::function<void()> _LevelReadyFunc) {
  UE_LOG(LogTemp, Warning, TEXT("Loading level: %s"), *UEnum::GetDisplayValueAsText(Level).ToString());

  if (ULevelStreaming* Streaming = UGameplayStatics::GetStreamingLevel(this, LevelNames[Level])) {
    LoadedLevel = Level;
    LevelReadyFunc = _LevelReadyFunc;

    HUD->StartLevelLoadingTransition([this, Level = Level, Streaming = Streaming]() {
      Streaming->OnLevelShown.Clear();
      Streaming->OnLevelShown.AddDynamic(this, &ALevelManager::OnLevelShown);

      FLatentActionInfo LatentInfo;
      LatentInfo.CallbackTarget = this;
      LatentInfo.UUID = 1;
      UGameplayStatics::LoadStreamLevel(this, LevelNames[Level], true, false, LatentInfo);
    });
  }
}

void ALevelManager::BeginUnloadLevel(ELevel Level, std::function<void()> _LevelUnloadedFunc) {
  UE_LOG(LogTemp, Warning, TEXT("Unloading level: %s"), *UEnum::GetDisplayValueAsText(Level).ToString());

  if (ULevelStreaming* Streaming = UGameplayStatics::GetStreamingLevel(this, LevelNames[CurrentLevel])) {
    LevelUnloadedFunc = _LevelUnloadedFunc;

    Streaming->OnLevelUnloaded.Clear();
    Streaming->OnLevelUnloaded.AddDynamic(this, &ALevelManager::OnLevelUnloaded);

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    UGameplayStatics::UnloadStreamLevel(this, LevelNames[Level], LatentInfo, false);
  }
}

void ALevelManager::OnLevelShown() {
  UE_LOG(LogTemp, Warning, TEXT("Level shown: %s"), *UEnum::GetDisplayValueAsText(LoadedLevel).ToString());

  if (CurrentLevel != ELevel::None) {
    SaveLevelState(CurrentLevel);
    BeginUnloadLevel(CurrentLevel);
  }
  CurrentLevel = LoadedLevel;

  InitLevel(CurrentLevel);
  if (LevelReadyFunc) LevelReadyFunc();
  LevelReadyFunc = nullptr;

  HUD->EndLevelLoadingTransition([this]() { EnterLevel(CurrentLevel); });
}
void ALevelManager::OnLevelUnloaded() {
  UE_LOG(LogTemp, Warning, TEXT("Level unloaded: %s"), *UEnum::GetDisplayValueAsText(CurrentLevel).ToString());

  if (LevelUnloadedFunc) LevelUnloadedFunc();
  LevelUnloadedFunc = nullptr;
}

void ALevelManager::ReloadCurrentLevel(std::function<void()> _LevelReadyFunc) {
  UE_LOG(LogTemp, Warning, TEXT("Reloading level"));

  if (ULevelStreaming* Streaming = UGameplayStatics::GetStreamingLevel(this, LevelNames[CurrentLevel])) {
    LevelReadyFunc = _LevelReadyFunc;

    Streaming->OnLevelUnloaded.Clear();
    Streaming->OnLevelUnloaded.AddDynamic(this, &ALevelManager::OnLevelUnloadedReload);

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    UGameplayStatics::UnloadStreamLevel(this, LevelNames[CurrentLevel], LatentInfo, false);
  }
}
void ALevelManager::OnLevelUnloadedReload() {
  if (ULevelStreaming* Streaming = UGameplayStatics::GetStreamingLevel(this, LevelNames[CurrentLevel])) {
    Streaming->OnLevelShown.Clear();
    Streaming->OnLevelShown.AddDynamic(this, &ALevelManager::OnLevelShownReload);

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.UUID = 1;
    UGameplayStatics::LoadStreamLevel(this, LevelNames[CurrentLevel], true, false, LatentInfo);
  }
}
void ALevelManager::OnLevelShownReload() {
  InitLevel(CurrentLevel);

  LevelReadyFunc();
  LevelReadyFunc = nullptr;
}

void ALevelManager::InitLevel(ELevel Level) {
  switch (Level) {
    case ELevel::Store:
      check(Store);
      Store->LoadStoreLevelState();

      if (StorePhaseManager->StorePhaseState != EStorePhaseState::Night)
        StorePhaseLightingManager->SetupStoreLevelDayLighting();
      if (StorePhaseManager->StorePhaseState == EStorePhaseState::Night)
        StorePhaseLightingManager->SetupStoreLevelNightLighting();
      break;
    case ELevel::Market:
      check(MarketLevel);
      MarketLevel->LoadLevelState(DayManager->bIsWeekend);

      if (StorePhaseManager->StorePhaseState == EStorePhaseState::Morning)
        StorePhaseLightingManager->SetupMarketLevelLighting();
      break;
    case ELevel::Church:
      check(ChurchLevel);
      ChurchLevel->LoadLevelState();

      if (StorePhaseManager->StorePhaseState == EStorePhaseState::Night)
        StorePhaseLightingManager->SetupChurchLevelNightLighting();
      break;
    default: checkNoEntry();
  }
}

void ALevelManager::EnterLevel(ELevel Level) {
  check(CutsceneManager && PlayerTags && MarketLevel);

  FGameplayTagContainer LevelCutsceneTags = StringTagsToContainer(
      {FName(FString::Printf(TEXT("Cutscene.%s"), *UEnum::GetDisplayValueAsText(Level).ToString()))});
  if (CutsceneManager->PlayPotentialCutscene(LevelCutsceneTags)) return;

  switch (Level) {
    case ELevel::Store: Store->EnterLevel(); break;
    case ELevel::Market: MarketLevel->EnterLevel(); break;
    case ELevel::Church: ChurchLevel->EnterLevel(); break;
    default: checkNoEntry();
  }
}

void ALevelManager::SaveLevelState(ELevel Level) {
  switch (Level) {
    case ELevel::Store:
      check(Store);
      Store->SaveStoreLevelState();
      break;
    case ELevel::Market:
      check(MarketLevel);
      MarketLevel->SaveLevelState();
      break;
    case ELevel::Church:
      check(ChurchLevel);
      ChurchLevel->SaveLevelState();
      break;
    default: checkNoEntry();
  }
}

void ALevelManager::ExpandStoreSwitchLevel() {
  Store->StoreLevelState.ActorSaveMap.Empty();
  Store->StoreLevelState.ComponentSaveMap.Empty();
  Store->StoreLevelState.ObjectSaveStates.Empty();
  LevelReadyFunc = [this]() { PlayerCommand->ResetPosition(); };
  LevelUnloadedFunc = [this]() {
    LevelNames[ELevel::Store] = StoreExpansionLevelMap[StoreExpansionManager->CurrentStoreExpansionLevelID];

    CurrentLevel = ELevel::None;
    InitLoadStore(LevelReadyFunc);
  };
  HUD->StartLevelLoadingTransition([this]() { BeginUnloadLevel(ELevel::Store, LevelUnloadedFunc); });
}