#include "PlayerCommand.h"
#include "store_playground/Player/PlayerZDCharacter.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/Dialogue/DialogueComponent.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Quest/QuestComponent.h"

void APlayerCommand::BeginPlay() { Super::BeginPlay(); }

void APlayerCommand::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void APlayerCommand::CommandDialogue(TArray<struct FDialogueData> DialogueArray) {
  check(PlayerCharacter);
  check(PlayerCharacter->PlayerBehaviourState == EPlayerState::Normal);

  PlayerCharacter->EnterDialogue(DialogueArray);
}

void APlayerCommand::CommandNegotiation(UCustomerAIComponent* CustomerAI,
                                        const UItemBase* Item,
                                        bool bIsQuestAssociated,
                                        UQuestComponent* QuestComponent) {
  check(PlayerCharacter->PlayerBehaviourState == EPlayerState::Normal);

  PlayerCharacter->EnterNegotiation(CustomerAI, Item, bIsQuestAssociated, QuestComponent);
}

void APlayerCommand::CommandQuest(UQuestComponent* QuestC,
                                  UDialogueComponent* DialogueC,
                                  UCustomerAIComponent* CustomerAI,
                                  const UItemBase* Item) {
  check(PlayerCharacter->PlayerBehaviourState == EPlayerState::Normal);

  PlayerCharacter->EnterQuest(QuestC, DialogueC, CustomerAI, Item);
}

void APlayerCommand::CommandCutscene(TArray<struct FDialogueData> DialogueArray) {
  check(PlayerCharacter);
  check(PlayerCharacter->PlayerBehaviourState == EPlayerState::Normal);

  PlayerCharacter->EnterCutscene(DialogueArray);
}