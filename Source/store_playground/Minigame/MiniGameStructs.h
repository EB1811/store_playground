// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "PaperFlipbook.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Sprite/SpriteStructs.h"
#include "MiniGameStructs.generated.h"

UENUM()
enum class EMiniGame : uint8 {
  Lootbox UMETA(DisplayName = "Lootbox"),
};
ENUM_RANGE_BY_COUNT(EMiniGame, 1);

USTRUCT()
struct FMiniGameAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  UTexture2D* Avatar;
  UPROPERTY(EditAnywhere)
  UStaticMesh* Mesh;
  UPROPERTY(EditAnywhere)
  TMap<ESimpleSpriteDirection, UPaperFlipbook*> Sprites;
};

USTRUCT()
struct FMiniGameInfo {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  EMiniGame MiniGame;

  UPROPERTY(EditAnywhere)
  float SpawnChance;

  UPROPERTY(EditAnywhere)
  TArray<FDialogueData> TutDialogues;  // * Tutorial dialogue.
  UPROPERTY(EditAnywhere)
  TArray<FDialogueData> InitDialogues;  // * Dialogue before starting.

  UPROPERTY(EditAnywhere)
  FText DisplayName;  // * Name of the minigame, displayed in the UI.
  UPROPERTY(EditAnywhere)
  FMiniGameAssetData AssetData;
};