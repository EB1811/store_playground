// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "SpriteStructs.generated.h"

UENUM()
enum class ESimpleSpriteAnimState : uint8 {
  Idle UMETA(DisplayName = "Idle"),
  Walk UMETA(DisplayName = "Walk"),
};

UENUM()
enum class ESimpleSpriteDirection : uint8 {
  Up UMETA(DisplayName = "Up"),
  Down UMETA(DisplayName = "Down"),
  Left UMETA(DisplayName = "Left"),
  Right UMETA(DisplayName = "Right"),
};
ENUM_RANGE_BY_COUNT(ESimpleSpriteDirection, 4);
