// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonetaryDetailTextWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UMonetaryDetailTextWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NameText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ValueText;
};