// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/AI/AIStructs.h"
#include "NpcDataStructs.generated.h"

// * Quests.
UENUM()
enum class EQuestAction : uint8 {
  Continue UMETA(DisplayName = "Continue"),
  End UMETA(DisplayName = "End"),
};
UENUM()
enum class EQuestChainType : uint8 {
  DialogueChain UMETA(DisplayName = "Dialogue"),
  DialogueChoice UMETA(DisplayName = "Dialogue Choice"),  // * Save choices made.
  Negotiation UMETA(DisplayName = "Negotiation"),         // * Save negotiation outcomes.
};

// UENUM()
// enum class EStartReqType : uint8 {
//   None UMETA(DisplayName = "None"),  // * Always a random chance of happening.
//   Time UMETA(DisplayName = "Time"),
//   Money UMETA(DisplayName = "Money"),
//   Inventory UMETA(DisplayName = "Inventory"),                        // * Using item ids.
//   CompletedQuests UMETA(DisplayName = "Completed Quests"),           // * Using quest ids.
//   MadeDialogueChoices UMETA(DisplayName = "Made Dialogue Choices"),  // * Using dialogue chain ids.
// };
// UENUM()
// enum class EStartReqOperators : uint8 {
//   None UMETA(DisplayName = "None"),
//   EqualTo UMETA(DisplayName = "Equal To"),
//   GreaterThan UMETA(DisplayName = "Greater Than"),
//   LessThan UMETA(DisplayName = "Less Than"),
//   Contains UMETA(DisplayName = "Contains"),
// };
// UENUM()
// enum class EStartReqsLogic : uint8 {
//   None UMETA(DisplayName = "None"),
//   And UMETA(DisplayName = "And"),
//   Or UMETA(DisplayName = "Or"),
// };
// USTRUCT()
// struct FQuestChainStartReq {
//   GENERATED_BODY()

//   UPROPERTY(EditAnywhere)
//   EStartReqType StartReqType;
//   UPROPERTY(EditAnywhere)
//   EStartReqOperators StartReqOperator;
//   UPROPERTY(EditAnywhere)
//   FName StartReqValue;  // * String, number, id, (id, ...).
// };
// USTRUCT()
// struct FQuestChainStartRequirements {
//   GENERATED_BODY()

//   UPROPERTY(EditAnywhere)
//   TArray<FQuestChainStartReq> StartReqs;
//   UPROPERTY(EditAnywhere)
//   EStartReqsLogic Logic;
// };

USTRUCT()
struct FQuestChainData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName QuestID;
  UPROPERTY(EditAnywhere)
  FName QuestChainID;  // * For tracking quest chains.
  UPROPERTY(EditAnywhere)
  EQuestChainType QuestChainType;

  UPROPERTY(EditAnywhere)
  FName DialogueChainID;
  UPROPERTY(EditAnywhere)
  ECustomerAction CustomerActionToPerform;
  UPROPERTY(EditAnywhere)
  TArray<FName> ActionRelevantIDs;  // * Item ids, WantedItemTypeIDs, etc.

  UPROPERTY(EditAnywhere)
  FName StartRequirementsFilter;  // * Filter string using DynamoDB like syntax.
                                  // * e.g., "Money > 1000 AND contains(Inventory, ["item_id"]).
  UPROPERTY(EditAnywhere)
  float StartChance;
  UPROPERTY(EditAnywhere)
  bool bIsRepeatable;

  UPROPERTY(EditAnywhere)
  EQuestAction QuestAction;
  UPROPERTY(EditAnywhere)
  int32 BranchesAmount;
  UPROPERTY(EditAnywhere)
  TArray<FName> BranchRequiredChoiceIDs;
};

USTRUCT()
struct FQuestChainDataRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName QuestID;
  UPROPERTY(EditAnywhere)
  FName QuestChainID;  // * For tracking quest chains.
  UPROPERTY(EditAnywhere)
  EQuestChainType QuestChainType;

  UPROPERTY(EditAnywhere)
  FName DialogueChainID;
  UPROPERTY(EditAnywhere)
  ECustomerAction CustomerAction;
  UPROPERTY(EditAnywhere)
  TArray<FName> ActionRelevantIDs;  // * Item ids, WantedItemTypeIDs, etc.

  UPROPERTY(EditAnywhere)
  FName StartRequirementsFilter;  // * Filter string using DynamoDB like syntax.
                                  // * e.g., "Money > 1000 AND contains(Inventory, ["item_id"]).
  UPROPERTY(EditAnywhere)
  float StartChance;
  UPROPERTY(EditAnywhere)
  bool bIsRepeatable;

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
};

USTRUCT()
struct FNpcAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  UTexture2D* Avatar;
  UPROPERTY(EditAnywhere)
  UStaticMesh* Mesh;
  // UPROPERTY(EditAnywhere)
  // UMaterialInterface* Material;
};

USTRUCT()
struct FUniqueNpcData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName NpcID;
  UPROPERTY(EditAnywhere)
  FName LinkedPopID;  // Fallback

  UPROPERTY(EditAnywhere)
  FText NpcName;

  UPROPERTY(EditAnywhere)
  FName SpawnRequirementsFilter;  // * Identical to FQuestChainData.
  UPROPERTY(EditAnywhere)
  float SpawnChanceWeight;

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
  FName NpcID;
  UPROPERTY(EditAnywhere)
  FName LinkedPopID;  // Fallback

  UPROPERTY(EditAnywhere)
  FText NpcName;

  UPROPERTY(EditAnywhere)
  FName SpawnRequirementsFilter;  // * Identical to FQuestChainData.
  UPROPERTY(EditAnywhere)
  float SpawnChanceWeight;

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