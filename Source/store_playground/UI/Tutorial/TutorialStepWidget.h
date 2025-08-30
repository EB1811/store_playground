// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Tutorial/TutorialStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "TutorialStepWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UTutorialStepWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TitleText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* BodyText;
  UPROPERTY(meta = (BindWidget))
  class UImage* TutorialImage;
  UPROPERTY(meta = (BindWidget))
  class UCommonVideoPlayer* TutorialVideoPlayer;
};