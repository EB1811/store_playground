// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActionWidget.h"
#include "NewsHudSlideWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UNewsHudSlideWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* SlideText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NotificationText;  // Temp
  // UPROPERTY(meta = (BindWidget))
  // class UImage* NotificationImage;

  UPROPERTY(EditAnywhere)
  const class ANewsGen* NewsGen;

  void RefreshUI();
  void InitUI(const class ANewsGen* _NewsGen);
};