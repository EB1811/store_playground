// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArticleWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UArticleWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UImage* Picture;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Title;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Text;
};