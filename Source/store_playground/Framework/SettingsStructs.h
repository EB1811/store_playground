// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Types/SlateEnums.h"
#include "SettingsStructs.generated.h"

UENUM()
enum class EGameDifficulty : uint8 {
  Easier UMETA(DisplayName = "Easier"),
  Normal UMETA(DisplayName = "Normal"),
  Harder UMETA(DisplayName = "Harder"),
};
ENUM_RANGE_BY_COUNT(EGameDifficulty, 3);

USTRUCT()
struct FGameSettings {
  GENERATED_BODY()

  UPROPERTY()
  EGameDifficulty Difficulty;

  UPROPERTY()
  bool bShowTutorials;
};