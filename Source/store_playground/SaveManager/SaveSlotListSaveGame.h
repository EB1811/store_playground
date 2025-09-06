#pragma once

#include "GameFramework/SaveGame.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "SaveSlotListSaveGame.generated.h"

// * Saving the list of save slots in the save game directory, to be used for displaying the list of save slots in the UI.

UCLASS()
class STORE_PLAYGROUND_API USaveSlotListSaveGame : public USaveGame {
  GENERATED_BODY()

public:
  USaveSlotListSaveGame() {}

  // ? Maybe turn into tmap so that this is actual save slots and not a list of saves.
  UPROPERTY()
  TArray<FSaveSlotData> SaveSlotList;  // * List of save slots.
  UPROPERTY()
  int32 MostRecentSaveSlotIndex;  // * Most recent save slot to be used for quick load.
  UPROPERTY()
  bool bHasAutoSave;  // * Whether an auto-save exists.
};