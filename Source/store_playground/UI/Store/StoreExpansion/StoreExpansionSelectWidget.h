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

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NameText;
  UPROPERTY(meta = (BindWidget))
  class UImage* PictureImage;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* PriceText;
  UPROPERTY(meta = (BindWidget))
  class UButton* SelectButton;

  UPROPERTY()
  FName ExpansionID;
  UPROPERTY()
  bool bIsSelected;

  UFUNCTION()
  void Select();

  void RefreshUI();
  void InitUI(const FStoreExpansionData& ExpansionData, std::function<void(FName)> _SelectUpgradeFunc);

  std::function<void(FName)> SelectFunc;
};
