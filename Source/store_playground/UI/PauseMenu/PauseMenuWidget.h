// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UPauseMenuWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class USaveSlotsWidget* SaveSlotsWidget;

  UPROPERTY(meta = (BindWidget))
  class UButton* SaveButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* LoadButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* QuitButton;

  UPROPERTY()
  class ASaveManager* SaveManagerRef;

  UFUNCTION()
  void OnSaveButtonClicked();
  UFUNCTION()
  void OnLoadButtonClicked();
  // UFUNCTION()
  // void OnQuitButtonClicked();

  void RefreshUI();
};