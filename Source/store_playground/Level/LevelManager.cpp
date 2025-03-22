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

void ALevelManager::LoadLevel(ELevel Level) {
  UE_LOG(LogTemp, Warning, TEXT("Loading level: %s"), *UEnum::GetDisplayValueAsText(Level).ToString());

  // FLatentActionInfo LatentInfo;
  // UGameplayStatics::LoadStreamLevel(this, LevelName, true, true, LatentInfo);

  ULevelStreaming* Streaming = UGameplayStatics::GetStreamingLevel(this, LevelNames[Level]);
  if (Streaming) {
    Streaming->OnLevelShown.Clear();
    Streaming->OnLevelShown.AddDynamic(this, &ALevelManager::InitLevel);
    LoadedLevel = Level;

    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.UUID = 1;
    UGameplayStatics::LoadStreamLevel(this, LevelNames[Level], true, true, LatentInfo);
  }
}

void ALevelManager::UnloadLevel(ELevel Level) {
  UE_LOG(LogTemp, Warning, TEXT("Unloading level: %s"), *UEnum::GetDisplayValueAsText(Level).ToString());

  FLatentActionInfo LatentInfo;
  UGameplayStatics::UnloadStreamLevel(this, LevelNames[Level], LatentInfo, true);
}

void ALevelManager::InitLevel() {
  switch (LoadedLevel) {
    case ELevel::Store:
      check(Store);
      Store->InitStockDisplays();
      break;
    case ELevel::Market:
      check(Market);
      Market->InitializeNPCStores();
      break;
  }
}