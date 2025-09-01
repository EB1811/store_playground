#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"
#include "DisabledStoreExpansionSelectWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UDisabledStoreExpansionSelectWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NameText;
  UPROPERTY(meta = (BindWidget))
  class UImage* PictureImage;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* PriceText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* LockedReasonText;
};
