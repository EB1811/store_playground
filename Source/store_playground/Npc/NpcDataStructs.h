// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PaperFlipbook.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/AI/AIStructs.h"
#include "store_playground/Sprite/SpriteStructs.h"
#include "NpcDataStructs.generated.h"

// * Quests.
UENUM()
enum class EQuestChainType : uint8 {
  Dialogue UMETA(DisplayName = "Dialogue"),
  Branch UMETA(DisplayName = "Branch"),
};
UENUM()
enum class EQuestAction : uint8 {
  Continue UMETA(DisplayName = "Continue"),
  SplitBranch UMETA(DisplayName = "Split Branch"),
  End UMETA(DisplayName = "End"),
};

UENUM()
enum class EQuestStartLocation : uint8 {
  Any UMETA(DisplayName = "Any"),
  Market UMETA(DisplayName = "Market"),
  Store UMETA(DisplayName = "Store"),
  Church UMETA(DisplayName = "Church"),
};
UENUM()
enum class EQuestOutcomeType : uint8 {
  DialogueChain UMETA(DisplayName = "Dialogue"),
  DialogueChoice UMETA(DisplayName = "Dialogue Choice"),  // * Save choices made.
  Negotiation UMETA(DisplayName = "Negotiation"),         // * Save negotiation outcomes.
};

USTRUCT()
struct FQuestChainData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName QuestID;
  UPROPERTY(EditAnywhere)
  FName ID;
  UPROPERTY(EditAnywhere, SaveGame)
  EQuestChainType QuestChainType;

  UPROPERTY(EditAnywhere, SaveGame)
  FName DialogueChainID;
  UPROPERTY(EditAnywhere, SaveGame)
  ECustomerAction CustomerAction;  // * What a customer does as part of the quest, if any.
  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FName> ActionRelevantIDs;  // * Item ids, WantedItemTypeIDs, etc.
  UPROPERTY(EditAnywhere, SaveGame)
  FName PostDialogueChainID;  // * Dialogue after the quest chain dialogue.

  UPROPERTY(EditAnywhere, SaveGame)
  EQuestStartLocation StartLocation;
  UPROPERTY(EditAnywhere, SaveGame)
  FName StartRequirementsFilter;  // * Filter string using DynamoDB like syntax.
                                  // * e.g., "Money > 1000 AND contains(Inventory, ["item_id"]).
  UPROPERTY(EditAnywhere, SaveGame)
  float StartChance;
  UPROPERTY(EditAnywhere, SaveGame)
  bool bIsRepeatable;

  UPROPERTY(EditAnywhere, SaveGame)
  EQuestOutcomeType QuestOutcomeType;  // * The relevant outcome saved.

  UPROPERTY(EditAnywhere, SaveGame)
  EQuestAction QuestAction;
  UPROPERTY(EditAnywhere, SaveGame)
  int32 BranchesAmount;
  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FName> BranchRequiredChoiceIDs;
};

USTRUCT()
struct FQuestChainDataRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName QuestID;
  UPROPERTY(EditAnywhere)
  FName ID;
  UPROPERTY(EditAnywhere)
  EQuestChainType QuestChainType;

  UPROPERTY(EditAnywhere)
  FName DialogueChainID;
  UPROPERTY(EditAnywhere)
  ECustomerAction CustomerAction;  // * What a customer does as part of the quest, if any.
  UPROPERTY(EditAnywhere)
  TArray<FName> ActionRelevantIDs;  // * Item ids, WantedItemTypeIDs, etc.
  UPROPERTY(EditAnywhere)
  FName PostDialogueChainID;  // * Dialogue after the quest chain dialogue.

  UPROPERTY(EditAnywhere)
  EQuestStartLocation StartLocation;
  UPROPERTY(EditAnywhere)
  FName StartRequirementsFilter;  // * Filter string using DynamoDB like syntax.
                                  // * e.g., "Money > 1000 AND contains(Inventory, ["item_id"]).
  UPROPERTY(EditAnywhere)
  float StartChance;
  UPROPERTY(EditAnywhere)
  bool bIsRepeatable;

  UPROPERTY(EditAnywhere)
  EQuestOutcomeType QuestOutcomeType;  // * The relevant outcome saved.

  UPROPERTY(EditAnywhere)
  EQuestAction QuestAction;
  UPROPERTY(EditAnywhere)
  int32 BranchesAmount;
  UPROPERTY(EditAnywhere)
  TArray<FName> BranchRequiredChoiceIDs;
};

USTRUCT()
struct FNpcNegotiationData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  ECustomerAttitude Attitude;
  UPROPERTY(EditAnywhere)
  TArray<float> AcceptancePercentageRange;  // * [Min, Max] for random generation.
  UPROPERTY(EditAnywhere)
  float MaxHagglingCount;

  UPROPERTY(EditAnywhere)
  FGameplayTagContainer Tags;
};

USTRUCT()
struct FNpcAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  UTexture2D* Avatar;
  UPROPERTY(EditAnywhere)
  TMap<ESimpleSpriteDirection, UPaperFlipbook*> IdleSprites;
  UPROPERTY(EditAnywhere)
  TMap<ESimpleSpriteDirection, UPaperFlipbook*> WalkSprites;
};

USTRUCT()
struct FUniqueNpcData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;
  UPROPERTY(EditAnywhere)
  FName LinkedPopID;  // Fallback

  UPROPERTY(EditAnywhere)
  FText NpcName;

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<EQuestStartLocation> SpawnableLocations;
  UPROPERTY(EditAnywhere)
  FName SpawnRequirementsFilter;  // * Identical to FQuestChainData.
  UPROPERTY(EditAnywhere)
  int32 SpawnWeight;

  // * NPC specific behaviour data.
  UPROPERTY(EditAnywhere)
  TArray<FName> QuestIDs;
  UPROPERTY(EditAnywhere)
  TArray<FName> DialogueChainIDs;  // * Dialogue used outside of a quest.

  UPROPERTY(EditAnywhere)
  FNpcNegotiationData NegotiationData;

  UPROPERTY(EditAnywhere)
  FNpcAssetData AssetData;
};

USTRUCT()
struct FUniqueNpcDataRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;
  UPROPERTY(EditAnywhere)
  FName LinkedPopID;  // Fallback

  UPROPERTY(EditAnywhere)
  FText NpcName;

  UPROPERTY(EditAnywhere)
  TArray<EQuestStartLocation> SpawnableLocations;
  UPROPERTY(EditAnywhere)
  FName SpawnRequirementsFilter;  // * Identical to FQuestChainData.
  UPROPERTY(EditAnywhere)
  int32 SpawnWeight;

  // * NPC specific behaviour data.
  UPROPERTY(EditAnywhere)
  TArray<FName> QuestIDs;
  UPROPERTY(EditAnywhere)
  TArray<FName> DialogueChainIDs;  // * Dialogue used outside of a quest.

  UPROPERTY(EditAnywhere)
  FNpcNegotiationData NegotiationData;

  UPROPERTY(EditAnywhere)
  FNpcAssetData AssetData;
};