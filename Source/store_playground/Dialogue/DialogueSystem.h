// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <optional>
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueSystem.generated.h"

UENUM()
enum class EDialogueState : uint8 {
  None UMETA(DisplayName = "NONE"),
  NPCTalk UMETA(DisplayName = "NPC Talk"),
  PlayerTalk UMETA(DisplayName = "Player Talk"),
  PlayerChoice UMETA(DisplayName = "Player Choice"),
  End UMETA(DisplayName = "End"),
};
UENUM()
enum class EDialogueAction : uint8 {
  NPCNext UMETA(DisplayName = "NPC Next"),
  PlayerNext UMETA(DisplayName = "Player Next"),
  AskPlayer UMETA(DisplayName = "Ask Player"),
  End UMETA(DisplayName = "End"),
};

UENUM()
enum class EDialogueSpeaker : uint8 {
  NPC UMETA(DisplayName = "NPC"),
  Player UMETA(DisplayName = "Player"),
};
USTRUCT()
struct FDialogueDataTable : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FString DialogueText;
  UPROPERTY(EditAnywhere)
  EDialogueSpeaker DialogueSpeaker;
  UPROPERTY(EditAnywhere)
  EDialogueAction Action;
};
USTRUCT()
struct FDialogueData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FString DialogueText;
  UPROPERTY(EditAnywhere)
  EDialogueSpeaker DialogueSpeaker;
  UPROPERTY(EditAnywhere)
  EDialogueAction Action;
};

struct NextDialogueRes {
  FString DialogueText;
  EDialogueState State;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API UDialogueSystem : public UObject {
  GENERATED_BODY()

public:
  UDialogueSystem() : DialogueState(EDialogueState::None), CurrentDialogueIndex(0) {}

  UPROPERTY(EditAnywhere, Category = "Dialogue")
  EDialogueState DialogueState;

  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TArray<FDialogueData> DialogueDataArr;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  int32 CurrentDialogueIndex;

  NextDialogueRes StartDialogue(const TArray<FDialogueData> _DialogueDataArr);
  NextDialogueRes NextDialogue();
};

EDialogueState GetNextDialogueState(EDialogueState CurrentState, EDialogueAction Action);