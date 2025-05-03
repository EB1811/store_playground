// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UMainMenuWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(meta = (BindWidget))
  class UButton* NewGameButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* ContinueButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* LoadGameButton;
  // UPROPERTY(meta = (BindWidget))
  // class UButton* QuitButton;
};