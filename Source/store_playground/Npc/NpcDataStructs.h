// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NpcDataStructs.generated.h"

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
  float SpawnChanceMultiplier;

  // * Dialogues that the NPC can have.
  // TODO: Think about ordering and branching.
  UPROPERTY(EditAnywhere)
  TArray<FName> DialogueChainIDs;
  UPROPERTY(EditAnywhere)
  TArray<FName> WantedBaseItemIDs;

  UPROPERTY(EditAnywhere)
  TArray<float> AcceptancePercentageRange;

  UPROPERTY(EditAnywhere)
  FNpcAssetData AssetData;
};

USTRUCT()
struct FUniqueNpcDataTable : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName NpcID;

  UPROPERTY(EditAnywhere)
  float SpawnChanceMultiplier;

  UPROPERTY(EditAnywhere)
  TArray<FName> DialogueChainIDs;
  UPROPERTY(EditAnywhere)
  TArray<FName> WantedBaseItemIDs;

  UPROPERTY(EditAnywhere)
  TArray<float> AcceptancePercentageRange;  // * [Min, Max] for random generation.

  UPROPERTY(EditAnywhere)
  FNpcAssetData AssetData;
};