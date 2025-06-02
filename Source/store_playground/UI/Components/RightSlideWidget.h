// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RightSlideWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API URightSlideWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* SlideText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* RightSlideText;  // Optional.

  // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
  // FText SlideText;
};