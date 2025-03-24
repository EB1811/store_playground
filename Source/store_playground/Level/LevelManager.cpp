#include "LevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "UObject/Class.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Store/Store.h"

ALevelManager::ALevelManager() { PrimaryActorTick.bCanEverTick = false; }

void ALevelManager::BeginPlay() {
  Super::BeginPlay();

  for (ELevel Level : TEnumRange<ELevel>()) check(LevelNames.Contains(Level));
}

void ALevelManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ALevelManager::BeginLoadLevel(ELevel Level, std::function<void()> _LevelReadyFunc) {
  UE_LOG(LogTemp, Warning, TEXT("Loading level: %s"), *UEnum::GetDisplayValueAsText(Level).ToString());

  if (ULevelStreaming* Streaming = UGameplayStatics::GetStreamingLevel(this, LevelNames[Level])) {
    LoadedLevel = Level;
    LevelReadyFunc = _LevelReadyFunc;

    Streaming->OnLevelShown.Clear();
    Streaming->OnLevelShown.AddDynamic(this, &ALevelManager::OnLevelShown);

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.UUID = 1;
    UGameplayStatics::LoadStreamLevel(this, LevelNames[Level], true, false, LatentInfo);
  }
}

void ALevelManager::BeginUnloadLevel(ELevel Level) {
  UE_LOG(LogTemp, Warning, TEXT("Unloading level: %s"), *UEnum::GetDisplayValueAsText(Level).ToString());

  FLatentActionInfo LatentInfo;
  UGameplayStatics::UnloadStreamLevel(this, LevelNames[Level], LatentInfo, false);
}

void ALevelManager::OnLevelShown() {
  InitLevel(LoadedLevel);

  if (LevelReadyFunc) LevelReadyFunc();
  LevelReadyFunc = nullptr;

  if (CurrentLevel != ELevel::None) {
    SaveLevelState(CurrentLevel);
    BeginUnloadLevel(CurrentLevel);
  }
  CurrentLevel = LoadedLevel;
}

void ALevelManager::InitLevel(ELevel Level) {
  switch (Level) {
    case ELevel::Store:
      check(Store);
      Store->LoadStoreLevelState();
      break;
    case ELevel::Market:
      check(Market);
      Market->LoadMarketLevelState();
      break;
  }
}

void ALevelManager::SaveLevelState(ELevel Level) {
  switch (Level) {
    case ELevel::Store:
      check(Store);
      Store->SaveStoreLevelState();
      break;
    case ELevel::Market:
      check(Market);
      Market->SaveMarketLevelState();
      break;
  }
}