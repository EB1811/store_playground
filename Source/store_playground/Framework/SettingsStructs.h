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

USTRUCT()
struct FAdvGraphicsSettings {
  GENERATED_BODY()

  UPROPERTY()
  int32 RenderMethod;  // 0: Default, 1: DirectX12, 2: Vulkan
  UPROPERTY()
  int32 AntiAliasingMethod;  // 0: None, 1: FXAA, 4: TSR, 5: DLSS
  UPROPERTY()
  int32 GlobalIlluminationMethod;  // 0: None, 1: Lumen, 2: Screen Space
  UPROPERTY()
  int32 ReflectionMethod;  // 0: None, 1: Lumen, 2: Screen Space
  UPROPERTY()
  bool bDepthOfField;
  UPROPERTY()
  bool bBloom;
  UPROPERTY()
  bool bDLSSFrameGeneration;
};