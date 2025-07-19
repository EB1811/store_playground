#include "LevelManager.h"
#include "Containers/ContainersFwd.h"
#include "Engine/LevelStreaming.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "UObject/Class.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/Level/LevelStructs.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Level/MarketLevel.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"
#include "store_playground/UI/SpgHUD.h"
#include "store_playground/Cutscene/CutsceneManager.h"

ALevelManager::ALevelManager() { PrimaryActorTick.bCanEverTick = false; }

void ALevelManager::BeginPlay() {
  Super::BeginPlay();

  for (ELevel Level : TEnumRange<ELevel>()) check(LevelNames.Contains(Level));
  for (EStoreExpansionLevel Expansion : TEnumRange<EStoreExpansionLevel>())
    check(StoreExpansionLevelNames.Contains(Expansion));

  HUD = Cast<ASpgHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
  check(HUD);
}

void ALevelManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ALevelManager::InitLoadStore(std::function<void()> _LevelReadyFunc) {
  LevelNames[ELevel::Store] = StoreExpansionLevelNames[StoreExpansionManager->CurrentStoreExpansionLevel];

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

void ALevelManager::BeginUnloadLevel(ELevel Level) {
  UE_LOG(LogTemp, Warning, TEXT("Unloading level: %s"), *UEnum::GetDisplayValueAsText(Level).ToString());

  FLatentActionInfo LatentInfo;
  UGameplayStatics::UnloadStreamLevel(this, LevelNames[Level], LatentInfo, false);
}

void ALevelManager::OnLevelShown() {
  UE_LOG(LogTemp, Warning, TEXT("Level shown: %s"), *UEnum::GetDisplayValueAsText(LoadedLevel).ToString());

  InitLevel(LoadedLevel);
  if (LevelReadyFunc) LevelReadyFunc();
  LevelReadyFunc = nullptr;

  if (CurrentLevel != ELevel::None) {
    SaveLevelState(CurrentLevel);
    BeginUnloadLevel(CurrentLevel);
  }
  CurrentLevel = LoadedLevel;

  HUD->EndLevelLoadingTransition([this]() { EnterLevel(CurrentLevel); });
}

void ALevelManager::ReloadCurrentLevel(std::function<void()> _LevelReadyFunc) {
  UE_LOG(LogTemp, Warning, TEXT("Reloading level"));

  if (ULevelStreaming* Streaming = UGameplayStatics::GetStreamingLevel(this, LevelNames[CurrentLevel])) {
    LevelReadyFunc = _LevelReadyFunc;

    Streaming->OnLevelUnloaded.Clear();
    Streaming->OnLevelUnloaded.AddDynamic(this, &ALevelManager::OnLevelUnloaded);

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    UGameplayStatics::UnloadStreamLevel(this, LevelNames[CurrentLevel], LatentInfo, false);
  }
}
void ALevelManager::OnLevelUnloaded() {
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
      break;
    case ELevel::Market:
      check(MarketLevel);
      MarketLevel->LoadLevelState(DayManager->bIsWeekend);
      break;
  }
}

void ALevelManager::EnterLevel(ELevel Level) {
  check(CutsceneManager && PlayerTags && MarketLevel);

  FGameplayTagContainer LevelCutsceneTags = StringTagsToContainer(
      {FName(FString::Printf(TEXT("Cutscene.%s"), *UEnum::GetDisplayValueAsText(Level).ToString()))});
  if (CutsceneManager->PlayPotentialCutscene(LevelCutsceneTags)) return;

  switch (Level) {
    case ELevel::Store: break;
    case ELevel::Market: MarketLevel->EnterLevel(); break;
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
  }
}

void ALevelManager::ExpandStoreSwitchLevel(std::function<void()> _LevelReadyFunc) {
  LevelReadyFunc = _LevelReadyFunc;
  HUD->StartLevelLoadingTransition([this]() {
    // todo-low: Add on unload callback.
    BeginUnloadLevel(ELevel::Store);

    LevelNames[ELevel::Store] = StoreExpansionLevelNames[StoreExpansionManager->CurrentStoreExpansionLevel];

    CurrentLevel = ELevel::None;
    InitLoadStore(LevelReadyFunc);
  });
}