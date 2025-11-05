// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueBoxWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UDialogueBoxWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* Speaker;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* DialogueText;
  UPROPERTY(meta = (BindWidget))
  class UButton* NextButton;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* NextButtonText;

  UPROPERTY(meta = (BindWidget))
  class UBorder* BgBorder;
};