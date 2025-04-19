#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Types/SlateEnums.h"
#include "LevelStructs.generated.h"

UENUM()
enum class ELevel : uint8 {
  None UMETA(DisplayName = "None"),
  Store UMETA(DisplayName = "Store"),
  Market UMETA(DisplayName = "Market"),
  Church UMETA(DisplayName = "Church"),
};
ENUM_RANGE_BY_COUNT(ELevel, 4);