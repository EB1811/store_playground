#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/NewsGen/NewsGenDataStructs.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "UpgradeWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UUpgradeWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY()
  FName UpgradeID;

  // UPROPERTY(meta = (BindWidget))
  // class UImage* ArticleImage;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* UpgradeNameText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* UpgradeDescText;
  UPROPERTY(meta = (BindWidget))
  class UButton* UpgradeButton;

  void SetUpgradeData(const FUpgrade& Upgrade);

  UFUNCTION()
  void OnUpgradeButtonClicked();
  std::function<void(FName)> SelectUpgradeFunc;
};
