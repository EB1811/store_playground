
#include "MySaveGame.h"
#include "Kismet/GameplayStatics.h"

UMySaveGame::UMySaveGame() { SlotName = TEXT("TestSaveSlot"); }

void UMySaveGame::Initialize(const FString& _SlotName, int32 _SlotIndex) {
  SlotName = _SlotName;
  SlotIndex = _SlotIndex;
  SystemSaveStates.Empty();
  LevelSaveStates.Empty();
  ActorSaveStates.Empty();
  ComponentSaveStates.Empty();
  ObjectSaveStates.Empty();
}