// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "GameOverViewWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UGameOverViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UButton* BackToMenuButton;

  UFUNCTION()
  void BackToMenu();

  void InitUI(FInUIInputActions InUIInputActions);
};