// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "GameplayTagContainer.h"
#include "store_playground/Tutorial/TutorialStructs.h"
#include "PlayerCommand.generated.h"

// * Interface for game systems to send commands to the player character (as opposed to the player controller).
// e.g., cutscene, dialogue, etc.
// ? Currently on demand. Could use a command queue using tick.

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API APlayerCommand : public AInfo {
  GENERATED_BODY()

public:
  APlayerCommand() { PrimaryActorTick.bCanEverTick = false; }

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  class APlayerZDCharacter* PlayerCharacter;
  UPROPERTY(EditAnywhere)
  class ASpgHUD* HUD;

  void ResetPosition();  // * Reset player position to the last spawn point.

  void CommandDialogue(TArray<struct FDialogueData> DialogueArray);
  void CommandNegotiation(class UCustomerAIComponent* CustomerAI,
                          class UItemBase* Item,
                          bool bIsQuestAssociated = false,
                          class UQuestComponent* QuestComponent = nullptr);
  void CommandQuest(class UQuestComponent* QuestC,
                    class UDialogueComponent* DialogueC,
                    class USimpleSpriteAnimComponent* SpriteAnimC,
                    class UCustomerAIComponent* CustomerAI = nullptr,
                    class UItemBase* Item = nullptr);
  void CommandCutscene(struct FResolvedCutsceneData ResolvedCutsceneData);
  auto CommandTutorial(const TArray<FUITutorialStep>& Steps) -> bool;

  void CommandExitCurrentAction();  // * Exit current action (e.g., dialogue, negotiation, etc.).
};