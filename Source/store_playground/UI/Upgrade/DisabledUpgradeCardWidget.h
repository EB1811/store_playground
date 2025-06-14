#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "DisabledUpgradeCardWidget.generated.h"

// * Used for both disabled upgrades, and unlocked upgrades.

UCLASS()
class STORE_PLAYGROUND_API UDisabledUpgradeCardWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* UpgradeNameText;
  UPROPERTY(meta = (BindWidget))
  class URichTextBlock* UpgradeDescText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* UpgradeCostText;
  UPROPERTY(meta = (BindWidget))
  class URichTextBlock* ReqsNotMetText;
};
