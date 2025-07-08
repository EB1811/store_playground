#include "PlayerCommand.h"
#include "store_playground/Player/PlayerZDCharacter.h"
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

void APlayerCommand::CommandDialogue(TArray<struct FDialogueData> DialogueArray) {
  check(PlayerCharacter);
  check(PlayerCharacter->PlayerBehaviourState == EPlayerState::Normal);

  PlayerCharacter->EnterDialogue(DialogueArray);
}

void APlayerCommand::CommandNegotiation(UCustomerAIComponent* CustomerAI,
                                        UItemBase* Item,
                                        bool bIsQuestAssociated,
                                        UQuestComponent* QuestComponent) {
  check(PlayerCharacter->PlayerBehaviourState == EPlayerState::Normal);

  PlayerCharacter->EnterNegotiation(CustomerAI, Item, bIsQuestAssociated, QuestComponent);
}

void APlayerCommand::CommandQuest(UQuestComponent* QuestC,
                                  UDialogueComponent* DialogueC,
                                  USimpleSpriteAnimComponent* SpriteAnimC,
                                  UCustomerAIComponent* CustomerAI,
                                  UItemBase* Item) {
  check(PlayerCharacter->PlayerBehaviourState == EPlayerState::Normal);

  PlayerCharacter->EnterQuest(QuestC, DialogueC, SpriteAnimC, CustomerAI, Item);
}

void APlayerCommand::CommandCutscene(struct FResolvedCutsceneData ResolvedCutsceneData, FGameplayTag CutsceneTag) {
  check(PlayerCharacter);
  check(PlayerCharacter->PlayerBehaviourState == EPlayerState::Normal);

  if (CutsceneTag.IsValid()) PlayerCharacter->PlayerTagsComponent->CutsceneTags.RemoveTag(CutsceneTag);

  PlayerCharacter->EnterCutscene(ResolvedCutsceneData);
}

void APlayerCommand::CommandExitCurrentAction() {
  check(PlayerCharacter);

  PlayerCharacter->ExitCurrentAction();
}