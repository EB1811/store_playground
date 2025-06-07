// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreMonetaryDetailCardWidget.generated.h"

USTRUCT()
struct FMonetaryDetail {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FText Text;
  UPROPERTY(EditAnywhere)
  float Value;
};

UCLASS()
class STORE_PLAYGROUND_API UStoreMonetaryDetailCardWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UMonetaryDetailTextWidget> MonetaryDetailTextWidgetClass;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TitleText;
  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* MonetaryDetailsPanelVerticalBox;

  void InitUI(FText Title, TArray<FMonetaryDetail> MonetaryDetails);
};