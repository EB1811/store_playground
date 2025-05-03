#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomerRequestingWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UCustomerRequestingWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* RequestingText;
};
