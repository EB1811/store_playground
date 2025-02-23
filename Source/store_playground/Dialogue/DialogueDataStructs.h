// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <optional>
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DialogueDataStructs.generated.h"

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
  None UMETA(DisplayName = "NONE"),
  NPCNext UMETA(DisplayName = "NPC Next"),
  PlayerNext UMETA(DisplayName = "Player Next"),
  AskPlayer UMETA(DisplayName = "Ask Player"),
  End UMETA(DisplayName = "End"),
};

UENUM()
enum class EDialogueSpeaker : uint8 {
  None UMETA(DisplayName = "NONE"),
  NPC UMETA(DisplayName = "NPC"),
  Player UMETA(DisplayName = "Player"),
};

UENUM()
enum class EDialogueType : uint8 {
  Dialogue UMETA(DisplayName = "Dialogue"),
  Choice UMETA(DisplayName = "Choice"),
};

// TODO: Change to class. (for pointers)
USTRUCT()
struct FDialogueData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName DialogueChainID;
  UPROPERTY(EditAnywhere)
  EDialogueType DialogueType;

  UPROPERTY(EditAnywhere)
  FString DialogueText;
  UPROPERTY(EditAnywhere)
  EDialogueAction Action;

  UPROPERTY(EditAnywhere)
  EDialogueSpeaker DialogueSpeaker;

  UPROPERTY(EditAnywhere)
  int32 ChoicesAmount;  // Note: Choices dialogues + response dialogues.
};

USTRUCT()
struct FNextDialogueRes {
  GENERATED_BODY()

  std::optional<FDialogueData> DialogueData;
  EDialogueState State;
};

USTRUCT()
struct FDialogueDataTable : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName DialogueChainID;
  UPROPERTY(EditAnywhere)
  EDialogueType DialogueType;

  UPROPERTY(EditAnywhere)
  FString DialogueText;
  UPROPERTY(EditAnywhere)
  EDialogueAction Action;

  UPROPERTY(EditAnywhere)
  EDialogueSpeaker DialogueSpeaker;

  UPROPERTY(EditAnywhere)
  int32 ChoicesAmount;  // Note: To know number of children in a preorder tree traversal.
  UPROPERTY(EditAnywhere)
  int32 ChoiceIndex;  // Note: This is really just for nicer understanding in the data table.
};

UENUM()
enum class ECustomerAttitude : uint8 {
  Friendly,
  Neutral,
  Hostile,
};

UENUM()
enum class ENegotiationDialogueType : uint8 {
  Request UMETA(DisplayName = "Request"),
  ConsiderTooHigh UMETA(DisplayName = "Consider Too High"),
  ConsiderClose UMETA(DisplayName = "Consider Close"),
  Accept UMETA(DisplayName = "Accept"),
  Reject UMETA(DisplayName = "Reject"),
};

USTRUCT()
struct FNegotiationDialoguesDataTable : public FTableRowBase {
  GENERATED_BODY()

  // Primary key.
  UPROPERTY(EditAnywhere)
  ECustomerAttitude Attitude;
  // Sort key.
  UPROPERTY(EditAnywhere)
  ENegotiationDialogueType NegotiationType;

  UPROPERTY(EditAnywhere)
  FName DialogueChainID;
  UPROPERTY(EditAnywhere)
  EDialogueType DialogueType;

  UPROPERTY(EditAnywhere)
  FString DialogueText;
  UPROPERTY(EditAnywhere)
  EDialogueAction Action;

  UPROPERTY(EditAnywhere)
  EDialogueSpeaker DialogueSpeaker;

  UPROPERTY(EditAnywhere)
  int32 ChoicesAmount;  // Note: To know number of children in a preorder tree traversal.
  UPROPERTY(EditAnywhere)
  int32 ChoiceIndex;  // Note: This is really just for nicer understanding in the data table.
};