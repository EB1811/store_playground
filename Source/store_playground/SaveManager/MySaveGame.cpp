
#include "MySaveGame.h"
#include "Kismet/GameplayStatics.h"

UMySaveGame::UMySaveGame() { SlotName = TEXT("TestSaveSlot"); }

void UMySaveGame::Initialize(const FString& _SlotName) {
  SlotName = _SlotName;
  SystemSaveStates.Empty();
  LevelSaveStates.Empty();
  ActorSaveStates.Empty();
  ComponentSaveStates.Empty();
  ObjectSaveStates.Empty();
}