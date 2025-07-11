
#include "AmbientSoundManager.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/WorldObject/Audio/WorldAudio.h"
#include "Engine/World.h"

void AAmbientSoundManager::BeginPlay() {
  Super::BeginPlay();

  check(AmbientSoundClass);
}

void AAmbientSoundManager::MorningSoundCalled() {
  auto WorldAudioActors = GetAllActorsOf<AWorldAudio>(GetWorld(), AmbientSoundClass);
  check(WorldAudioActors.Num() > 0);

  for (AWorldAudio* WorldAudio : WorldAudioActors) {
    if (!WorldAudio->ActorHasTag(AmbientSoundManagerParams.MorningSoundTag) &&
        !WorldAudio->ActorHasTag(AmbientSoundManagerParams.ShopOpenSoundTag) &&
        !WorldAudio->ActorHasTag(AmbientSoundManagerParams.NightSoundTag))
      continue;

    if (WorldAudio->ActorHasTag(AmbientSoundManagerParams.MorningSoundTag))
      WorldAudio->AudioComponent->FadeIn(0.5f, 1.0f, 0.0f, EAudioFaderCurve::Linear);
    else WorldAudio->AudioComponent->FadeOut(0.5f, 0.0f, EAudioFaderCurve::Linear);
  }
}
void AAmbientSoundManager::ShopOpenSoundCalled() {
  auto WorldAudioActors = GetAllActorsOf<AWorldAudio>(GetWorld(), AmbientSoundClass);
  check(WorldAudioActors.Num() > 0);

  for (AWorldAudio* WorldAudio : WorldAudioActors) {
    if (!WorldAudio->ActorHasTag(AmbientSoundManagerParams.MorningSoundTag) &&
        !WorldAudio->ActorHasTag(AmbientSoundManagerParams.ShopOpenSoundTag) &&
        !WorldAudio->ActorHasTag(AmbientSoundManagerParams.NightSoundTag))
      continue;

    if (WorldAudio->ActorHasTag(AmbientSoundManagerParams.ShopOpenSoundTag))
      WorldAudio->AudioComponent->FadeIn(0.5f, 1.0f, 0.0f, EAudioFaderCurve::Linear);
    else WorldAudio->AudioComponent->FadeOut(0.5f, 0.0f, EAudioFaderCurve::Linear);
  }
}
void AAmbientSoundManager::NightSoundCalled() {
  auto WorldAudioActors = GetAllActorsOf<AWorldAudio>(GetWorld(), AmbientSoundClass);
  check(WorldAudioActors.Num() > 0);

  for (AWorldAudio* WorldAudio : WorldAudioActors) {
    if (!WorldAudio->ActorHasTag(AmbientSoundManagerParams.MorningSoundTag) &&
        !WorldAudio->ActorHasTag(AmbientSoundManagerParams.ShopOpenSoundTag) &&
        !WorldAudio->ActorHasTag(AmbientSoundManagerParams.NightSoundTag))
      continue;

    if (WorldAudio->ActorHasTag(AmbientSoundManagerParams.NightSoundTag))
      WorldAudio->AudioComponent->FadeIn(0.5f, 1.0f, 0.0f, EAudioFaderCurve::Linear);
    else WorldAudio->AudioComponent->FadeOut(0.5f, 0.0f, EAudioFaderCurve::Linear);
  }
}