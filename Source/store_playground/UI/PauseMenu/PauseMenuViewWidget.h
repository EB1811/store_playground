// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "PauseMenuViewWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UPauseMenuViewWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UOverlay* MainOverlay;
  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class USaveLoadSlotsWidget* SaveLoadSlotsWidget;
  UPROPERTY(meta = (BindWidget))
  class USettingsWidget* SettingsWidget;

  UPROPERTY(meta = (BindWidget))
  class UButton* ResumeButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* SaveButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* LoadButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* SettingsButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* QuitButton;

  UPROPERTY(EditAnywhere)
  class USoundBase* HoverSound;
  UPROPERTY(EditAnywhere)
  class USoundBase* SelectSound;

  UPROPERTY(EditAnywhere)
  FInUIInputActions InUIInputActions;

  UPROPERTY(EditAnywhere)
  class ASettingsManager* SettingsManager;
  UPROPERTY()
  class ASaveManager* SaveManager;

  UPROPERTY(EditAnywhere)
  class UButton* HoveredButton;
  void SelectHoveredButton();
  void HoverButton(UButton* Button);
  void HoverNextButton(FVector2D Direction);

  UFUNCTION()
  void UnhoverButton();

  UFUNCTION()
  void Resume();
  UFUNCTION()
  void SaveMenu();
  UFUNCTION()
  void LoadMenu();
  UFUNCTION()
  void SettingsMenu();
  UFUNCTION()
  void Quit();

  void RefreshUI();
  void InitUI(FInUIInputActions _InUIInputActions,
              class ASettingsManager* _SettingsManager,
              class ASaveManager* _SaveManager,
              std::function<void()> _CloseWidgetFunc);

  // * Pause state currently doesn't have inputs.
  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  std::function<void()> CloseWidgetFunc;
};