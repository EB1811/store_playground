// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreDetailCardWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UStoreDetailCardWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TitleText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* SecondaryTitleText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Text1;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Text2;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* SecondaryText;

  void InitUI(FText Title,
              FText SecondaryTitle = FText::GetEmpty(),
              FText _Text1 = FText::GetEmpty(),
              FText _Text2 = FText::GetEmpty(),
              FText _SecondaryText = FText::GetEmpty());
};