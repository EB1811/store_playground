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

UENUM()
enum class EStoreExpansionLevel : uint8 {
  Store0 UMETA(DisplayName = "Store0"),
  Store1 UMETA(DisplayName = "Store1"),
  Store2 UMETA(DisplayName = "Store2"),
  Store3 UMETA(DisplayName = "Store3"),
  Store4 UMETA(DisplayName = "Store4"),
  Store5 UMETA(DisplayName = "Store5"),
};
ENUM_RANGE_BY_COUNT(EStoreExpansionLevel, 6);