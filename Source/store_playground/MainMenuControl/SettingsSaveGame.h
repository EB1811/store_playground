#pragma once

#include "GameFramework/SaveGame.h"
#include "store_playground/Framework/StorePGGameInstance.h"
#include "SettingsSaveGame.generated.h"

UCLASS()
class STORE_PLAYGROUND_API USettingsSaveGame : public USaveGame {
  GENERATED_BODY()

public:
  USettingsSaveGame(){};

  UPROPERTY()
  FString SlotName;

  UPROPERTY(EditAnywhere)
  FGameSettings GameSettings;
};