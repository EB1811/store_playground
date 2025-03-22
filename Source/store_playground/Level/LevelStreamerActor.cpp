#include "LevelStreamerActor.h"
#include "Kismet/GameplayStatics.h"
#include "LevelStreamerActor.h"
#include "store_playground/Level/LevelManager.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "store_playground/Market/Market.h"

ALevelStreamerActor::ALevelStreamerActor() {
  PrimaryActorTick.bCanEverTick = false;

  OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
  RootComponent = OverlapVolume;

  // OverlapVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  // OverlapVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
  // OverlapVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

  OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelStreamerActor::OverlapBegins);
  OverlapVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &ALevelStreamerActor::OverlapEnds);
}

void ALevelStreamerActor::BeginPlay() { Super::BeginPlay(); }

void ALevelStreamerActor::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ALevelStreamerActor::OverlapBegins(UPrimitiveComponent* OverlappedComponent,
                                        AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp,
                                        int32 OtherBodyIndex,
                                        bool bFromSweep,
                                        const FHitResult& SweepResult) {
  ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

  if (OtherActor != MyCharacter) return;

  TArray<AActor*> FoundActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), LevelManagerClass, FoundActors);
  ALevelManager* LevelManager = Cast<ALevelManager>(FoundActors[0]);
  LevelManager->LoadLevel(LevelToLoad);
}
void ALevelStreamerActor::OverlapEnds(UPrimitiveComponent* OverlappedComponent,
                                      AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex) {
  ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

  if (OtherActor != MyCharacter) return;

  TArray<AActor*> FoundActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), LevelManagerClass, FoundActors);
  ALevelManager* LevelManager = Cast<ALevelManager>(FoundActors[0]);
  LevelManager->UnloadLevel(LevelToUnload);
}