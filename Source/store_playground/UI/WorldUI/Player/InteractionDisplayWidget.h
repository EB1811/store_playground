#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionDisplayWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UInteractionDisplayWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* InteractionText;
};
