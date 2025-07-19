// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <optional>
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Misc/EnumRange.h"
#include "store_playground/Level/LevelStructs.h"
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

// * Dialogue tags for systems to filter further.
// Mostly for systems that retrieve random dialogues.
// UENUM()
// enum class EDialogueTag : uint8 {
//   LevelStore UMETA(DisplayName = "LevelStore"),
//   LevelMarket UMETA(DisplayName = "LevelMarket"),
//   LevelChurch UMETA(DisplayName = "LevelChurch"),
//   Idle UMETA(DisplayName = "Idle"),
// };

USTRUCT()
struct FDialogueData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  FName DialogueChainID;
  UPROPERTY(EditAnywhere, SaveGame)
  FName DialogueID;
  UPROPERTY(EditAnywhere, SaveGame)
  EDialogueType DialogueType;

  UPROPERTY(EditAnywhere, SaveGame)
  FText DialogueText;
  UPROPERTY(EditAnywhere, SaveGame)
  EDialogueAction Action;

  UPROPERTY(EditAnywhere, SaveGame)
  EDialogueSpeaker DialogueSpeaker;
  UPROPERTY(EditAnywhere, SaveGame)
  FText SpeakerName;  // * Override

  UPROPERTY(EditAnywhere, SaveGame)
  int32 ChoicesAmount;

  UPROPERTY(EditAnywhere, SaveGame)
  FGameplayTagContainer DialogueTags;
};
USTRUCT()
struct FDialogueDataTable : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName DialogueChainID;
  UPROPERTY(EditAnywhere)
  FName DialogueID;
  UPROPERTY(EditAnywhere)
  EDialogueType DialogueType;

  UPROPERTY(EditAnywhere)
  FText DialogueText;
  UPROPERTY(EditAnywhere)
  EDialogueAction Action;

  UPROPERTY(EditAnywhere)
  EDialogueSpeaker DialogueSpeaker;
  UPROPERTY(EditAnywhere, SaveGame)
  FText SpeakerName;  // * Override

  UPROPERTY(EditAnywhere)
  int32 ChoicesAmount;  // Note: To know number of children in a preorder tree traversal.
  UPROPERTY(EditAnywhere)
  int32 ChoiceIndex;  // Note: This is really just for nicer understanding in the data table.

  UPROPERTY(EditAnywhere)
  FGameplayTagContainer DialogueTags;
};

USTRUCT()
struct FNextDialogueRes {
  GENERATED_BODY()

  TOptional<FDialogueData> DialogueData;
  EDialogueState State;
};

// ? replace with tags?
UENUM()
enum class ECustomerAttitude : uint8 {
  Friendly,
  Neutral,
  Hostile,
};

UENUM()
enum class NegDialogueVars : uint8 {
  WantedItemType UMETA(DisplayName = "WantedItemType"),
  ItemPrice UMETA(DisplayName = "ItemPrice")
};
ENUM_RANGE_BY_COUNT(NegDialogueVars, 2);

UENUM()
enum class ENegotiationDialogueType : uint8 {
  BuyItemRequest UMETA(DisplayName = "Buy Item Request"),
  BuyItemTooHigh UMETA(DisplayName = "Buy Item Too High"),
  BuyItemClose UMETA(DisplayName = "Buy Item Close"),

  SellItemRequest UMETA(DisplayName = "Sell Item Request"),
  SellItemTooLow UMETA(DisplayName = "Sell Item Too Low"),
  SellItemClose UMETA(DisplayName = "Sell Item Close"),

  Accept UMETA(DisplayName = "Accept"),
  Reject UMETA(DisplayName = "Reject"),

  StockCheckRequest UMETA(DisplayName = "Stock Check Request"),
  StockCheckAccept UMETA(DisplayName = "Stock Check Accept"),
  StockCheckReject UMETA(DisplayName = "Stock Check Reject"),
};
ENUM_RANGE_BY_COUNT(ENegotiationDialogueType, 11);

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
  FName DialogueID;
  UPROPERTY(EditAnywhere)
  EDialogueType DialogueType;

  UPROPERTY(EditAnywhere)
  FText DialogueText;
  UPROPERTY(EditAnywhere)
  EDialogueAction Action;

  UPROPERTY(EditAnywhere)
  EDialogueSpeaker DialogueSpeaker;

  UPROPERTY(EditAnywhere)
  int32 ChoicesAmount;  // Note: To know number of children in a preorder tree traversal.
  UPROPERTY(EditAnywhere)
  int32 ChoiceIndex;  // Note: This is really just for nicer understanding in the data table.

  UPROPERTY(EditAnywhere)
  FGameplayTagContainer DialogueTags;
};

// * This enables using a list in a map.
USTRUCT()
struct FDialoguesArray {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  TArray<struct FDialogueData> Dialogues;
};