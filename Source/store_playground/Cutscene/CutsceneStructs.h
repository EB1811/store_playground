// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "CutsceneStructs.generated.h"

// * Only dialogue for this.
UENUM()
enum class ECutsceneChainType : uint8 {
  Dialogue UMETA(DisplayName = "Dialogue"),
  Movement UMETA(DisplayName = "Movement"),
  Animation UMETA(DisplayName = "Animation"),
  NpcEnter UMETA(DisplayName = "Npc Enter"),
  NpcExit UMETA(DisplayName = "Npc Exit"),
  CameraZoom UMETA(DisplayName = "Camera Zoom"),
  CameraPan UMETA(DisplayName = "Camera Pan"),
};

USTRUCT()
struct FCutsceneChainData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;
  UPROPERTY(EditAnywhere)
  FName CutsceneID;

  UPROPERTY(EditAnywhere)
  ECutsceneChainType CutsceneChainType;

  UPROPERTY(EditAnywhere)
  FName RelevantId;  // * Dialogue chain id, npc id, etc.
  UPROPERTY(EditAnywhere)
  FVector RelevantLocation;  // * For movement and camera pan.
};
USTRUCT()
struct FCutsceneChainDataRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;
  UPROPERTY(EditAnywhere)
  FName CutsceneID;

  UPROPERTY(EditAnywhere)
  ECutsceneChainType CutsceneChainType;

  UPROPERTY(EditAnywhere)
  FName RelevantId;  // * Dialogue chain id, npc id, etc.
  UPROPERTY(EditAnywhere)
  FVector RelevantLocation;  // * For movement and camera pan.
};

USTRUCT()
struct FCutsceneData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  FGameplayTagContainer PlayerTagsRequirements;
  UPROPERTY(EditAnywhere)
  int32 Priority;  // * Lowest int first.

  UPROPERTY(EditAnywhere)
  TArray<FName> StartingNpcIDs;  // * Npc ids that are relevant to the cutscene.
  UPROPERTY(EditAnywhere)
  TArray<FVector> StartingNpcLocations;  // * Npc locations that are relevant to the cutscene.

  UPROPERTY(EditAnywhere)
  FGameplayTagContainer Tags;  // * To filter.
};
USTRUCT()
struct FCutsceneDataRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;

  UPROPERTY(EditAnywhere)
  FGameplayTagContainer PlayerTagsRequirements;
  UPROPERTY(EditAnywhere)
  int32 Priority;  // * Lowest int first.

  UPROPERTY(EditAnywhere)
  TArray<FName> StartingNpcIDs;  // * Npc ids that are relevant to the cutscene.
  UPROPERTY(EditAnywhere)
  TArray<FVector> StartingNpcLocations;  // * Npc locations that are relevant to the cutscene.

  UPROPERTY(EditAnywhere)
  FGameplayTagContainer Tags;  // * To filter.
};

USTRUCT()
struct FResolvedCutsceneData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FCutsceneData CutsceneData;
  UPROPERTY(EditAnywhere)
  TArray<FCutsceneChainData> CutsceneChains;
  UPROPERTY(EditAnywhere)
  TArray<FDialogueData> Dialogues;
};