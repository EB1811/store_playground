#include "LevelInitializer.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/Market.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

ALevelInitializer::ALevelInitializer() {
  PrimaryActorTick.bCanEverTick = false;
  LevelInitializerType = ELevelInitializerType::None;
}

void ALevelInitializer::BeginPlay() {
  Super::BeginPlay();

  check(RelevantActorClass);

  switch (LevelInitializerType) {
    case ELevelInitializerType::Store: {
      InitializeStoreLevel();
      break;
    }
    case ELevelInitializerType::Market: {
      InitializeMarketLevel();
      break;
    }
    default: checkNoEntry();
  }
}

void ALevelInitializer::InitializeStoreLevel() {
  TArray<AActor*> FoundActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), RelevantActorClass, FoundActors);
  UE_LOG(LogTemp, Warning, TEXT("Found %d stores"), FoundActors.Num());

  for (AActor* Actor : FoundActors) {
    AStore* Store = Cast<AStore>(Actor);
    check(Store);

    Store->InitStockDisplays();
  }
}

void ALevelInitializer::InitializeMarketLevel() {
  TArray<AActor*> FoundActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), RelevantActorClass, FoundActors);
  UE_LOG(LogTemp, Warning, TEXT("Found %d markets"), FoundActors.Num());

  for (AActor* Actor : FoundActors) {
    AMarket* Market = Cast<AMarket>(Actor);
    check(Market);

    Market->InitNPCStores();
  }
}