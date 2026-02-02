// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NewspaperWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UNewspaperWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UArticleWidget> LargeArticleWidgetClass;
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UArticleWidget> MediumArticleWidgetClass;
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UArticleWidget> SmallArticleWidgetClass;

  UPROPERTY(EditAnywhere)
  const class ADayManager* DayManager;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TitleText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* DayText;
  UPROPERTY(meta = (BindWidget))
  class UWrapBox* NewspaperPanelWrapBox;

  UPROPERTY(EditAnywhere)
  class ANewsGen* NewsGen;

  UPROPERTY(EditAnywhere)
  int32 LookingAtDay;

  void ShowNextDay();
  void ShowPrevDay();

  void RefreshUI();
  void InitUI(const class ADayManager* _DayManager, class ANewsGen* _NewsGen);
};