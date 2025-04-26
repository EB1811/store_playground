#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"
#include "StoreExpansionSelectWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UStoreExpansionSelectWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY()
  EStoreExpansionLevel StoreExpansionLevel;

  // UPROPERTY(meta = (BindWidget))
  // class UImage* ArticleImage;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NameText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* PriceText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* IsLockedText;
  UPROPERTY(meta = (BindWidget))
  class UButton* SelectButton;

  void SetExpansionData(const FStoreExpansionData& Expansion);

  UFUNCTION()
  void OnSelectButtonClicked();
  std::function<void(EStoreExpansionLevel)> SelectExpansionFunc;
};
