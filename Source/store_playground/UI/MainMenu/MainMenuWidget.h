// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UMainMenuWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class USaveLoadSlotsWidget* SaveLoadSlotsWidget;
  UPROPERTY(meta = (BindWidget))
  class USettingsWidget* SettingsWidget;

  UPROPERTY(meta = (BindWidget))
  class UButton* ContinueButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* NewGameButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* LoadMenuButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* SettingsButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* ExitButton;

  UPROPERTY(EditAnywhere)
  FInUIInputActions InUIInputActions;

  UPROPERTY(EditAnywhere)
  class ASettingsManager* SettingsManager;
  UPROPERTY()
  class ASaveManager* SaveManager;

  UFUNCTION()
  void Continue();
  UFUNCTION()
  void NewGame();
  UFUNCTION()
  void LoadMenu();
  UFUNCTION()
  void SettingsMenu();
  UFUNCTION()
  void Exit();

  void RefreshUI();
  void InitUI(FInUIInputActions _InUIInputActions,
              class ASettingsManager* _SettingsManager,
              class ASaveManager* _SaveManager);

  // * Pause state currently doesn't have inputs.
  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();
};