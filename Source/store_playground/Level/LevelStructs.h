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
inline auto GetLevelText(ELevel Level) -> FText {
  switch (Level) {
    case ELevel::Store: return FText::FromString("Store");
    case ELevel::Market: return FText::FromString("Market");
    case ELevel::Church: return FText::FromString("Church");
    default: return FText::FromString("");
  }
}