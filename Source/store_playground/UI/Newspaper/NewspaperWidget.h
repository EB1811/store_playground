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
  TSubclassOf<class UNewspaperArticleWidget> NewspaperArticleClass;
  UPROPERTY(EditAnywhere)
  const class ANewsGen* NewsGenRef;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NewspaperTitleText;
  UPROPERTY(meta = (BindWidget))
  class UWrapBox* NewspaperPanelWrapBox;

  void RefreshNewspaperUI();
};