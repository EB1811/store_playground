#include "PlayerCommand.h"
#include "Logging/LogVerbosity.h"
#include "store_playground/Player/PlayerZDCharacter.h"
#include "store_playground/UI/SpgHUD.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Quest/QuestComponent.h"
#include "store_playground/Cutscene/CutsceneStructs.h"
#include "store_playground/Tags/TagsComponent.h"
#include "store_playground/Sprite/SimpleSpriteAnimComponent.h"

void APlayerCommand::BeginPlay() { Super::BeginPlay(); }

void APlayerCommand::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void APlayerCommand::ResetPosition() {
  check(PlayerCharacter);

  PlayerCharacter->ResetLocationToSpawnPoint();
}

auto APlayerCommand::CommandDialogue(TArray<struct FDialogueData> DialogueArray) -> bool {
  check(PlayerCharacter);
  check(PlayerCharacter->PlayerBehaviourState == EPlayerState::Normal);

  PlayerCharacter->EnterDialogue(DialogueArray);
  return true;
}

auto APlayerCommand::CommandNegotiation(UCustomerAIComponent* CustomerAI,
                                        UItemBase* Item,
                                        bool bIsQuestAssociated,
                                        UQuestComponent* QuestComponent) -> bool {
  check(PlayerCharacter->PlayerBehaviourState == EPlayerState::Normal);

  PlayerCharacter->EnterNegotiation(CustomerAI, Item, bIsQuestAssociated, QuestComponent);
  return true;
}

auto APlayerCommand::CommandQuest(UQuestComponent* QuestC,
                                  UDialogueComponent* DialogueC,
                                  USimpleSpriteAnimComponent* SpriteAnimC,
                                  UCustomerAIComponent* CustomerAI,
                                  UItemBase* Item) -> bool {
  check(PlayerCharacter->PlayerBehaviourState == EPlayerState::Normal);

  PlayerCharacter->EnterQuest(QuestC, DialogueC, SpriteAnimC, CustomerAI, Item);
  return true;
}

auto APlayerCommand::CommandCutscene(struct FResolvedCutsceneData ResolvedCutsceneData) -> bool {
  check(PlayerCharacter);
  if (PlayerCharacter->PlayerBehaviourState != EPlayerState::Normal &&
      PlayerCharacter->PlayerBehaviourState != EPlayerState::FocussedMenu) {
    UE_LOG(LogTemp, Error, TEXT("Cannot enter cutscene, player in state: %s"),
           *UEnum::GetValueAsString(PlayerCharacter->PlayerBehaviourState));
    return false;
  }

  CommandExitCurrentAction();
  PlayerCharacter->EnterCutscene(ResolvedCutsceneData);
  return true;
}

auto APlayerCommand::CommandTutorial(const TArray<FUITutorialStep>& Steps) -> bool {
  if (PlayerCharacter->PlayerBehaviourState != EPlayerState::Normal &&
      PlayerCharacter->PlayerBehaviourState != EPlayerState::FocussedMenu) {
    UE_LOG(LogTemp, Error, TEXT("Cannot start tutorial, player in state: %s"),
           *UEnum::GetValueAsString(PlayerCharacter->PlayerBehaviourState));
    return false;
  }

  HUD->SetAndOpenTutorialView(Steps);
  return true;
}

auto APlayerCommand::CommandExitCurrentAction() -> bool {
  check(PlayerCharacter);
  if (PlayerCharacter->PlayerBehaviourState != EPlayerState::Normal &&
      PlayerCharacter->PlayerBehaviourState != EPlayerState::FocussedMenu) {
    UE_LOG(LogTemp, Error, TEXT("Cannot exit current action, player in state: %s"),
           *UEnum::GetValueAsString(PlayerCharacter->PlayerBehaviourState));
    return false;
  }

  PlayerCharacter->ExitCurrentAction();
  return true;
}