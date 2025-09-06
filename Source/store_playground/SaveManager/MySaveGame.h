#pragma once

#include "GameFramework/SaveGame.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "MySaveGame.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UMySaveGame : public USaveGame {
  GENERATED_BODY()

public:
  UMySaveGame();

  UPROPERTY()
  FString SlotName;
  UPROPERTY()
  int32 SlotIndex;  // * -1 for auto save.

  UPROPERTY()
  TArray<FSystemSaveState> SystemSaveStates;
  UPROPERTY()
  TArray<FLevelSaveState> LevelSaveStates;
  UPROPERTY()
  TArray<FActorSavaState> ActorSaveStates;
  UPROPERTY()
  TArray<FComponentSaveState> ComponentSaveStates;
  UPROPERTY()
  TArray<FObjectSaveState> ObjectSaveStates;
  UPROPERTY()
  FPlayerSavaState PlayerSaveState;

  void Initialize(const FString& _SlotName, int32 _SlotIndex);
};