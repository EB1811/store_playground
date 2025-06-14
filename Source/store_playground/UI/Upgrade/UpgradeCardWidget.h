#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "UpgradeCardWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UUpgradeCardWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* UpgradeNameText;
  UPROPERTY(meta = (BindWidget))
  class URichTextBlock* UpgradeDescText;
  UPROPERTY(meta = (BindWidget))
  class URichTextBlock* UpgradeEffectsRichText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* UpgradeCostText;
  UPROPERTY(meta = (BindWidget))
  class UBorder* CardBorder;
  UPROPERTY(meta = (BindWidget))
  class UButton* SelectButton;

  UPROPERTY()
  FName UpgradeID;
  UPROPERTY()
  bool bIsSelected;

  void RefreshUI();
  void InitUI(const FUpgrade& Upgrade,
              TArray<FUpgradeEffect> UpgradeEffects,
              std::function<void(FName)> _SelectUpgradeFunc);

  UFUNCTION()
  void OnSelectButtonClicked();
  std::function<void(FName)> SelectFunc;
};
