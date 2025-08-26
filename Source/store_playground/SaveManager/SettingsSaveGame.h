#pragma once

#include "GameFramework/SaveGame.h"
#include "store_playground/Framework/SettingsStructs.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "SettingsSaveGame.generated.h"

UCLASS()
class STORE_PLAYGROUND_API USettingsSaveGame : public USaveGame {
  GENERATED_BODY()

public:
  USettingsSaveGame() {}

  UPROPERTY()
  FGameSettings GameSettings;

  UPROPERTY()
  FSavedSoundSettings SoundSettings;
};