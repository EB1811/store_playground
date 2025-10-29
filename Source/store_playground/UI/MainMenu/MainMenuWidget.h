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

  UPROPERTY(meta = (BindWidget))
  class UOverlay* MenusOverlay;
  UPROPERTY(meta = (BindWidget))
  class UNewGameSetupWidget* NewGameSetupWidget;
  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class USaveLoadSlotsWidget* SaveLoadSlotsWidget;
  UPROPERTY(meta = (BindWidget))
  class USettingsWidget* SettingsWidget;
  UPROPERTY(meta = (BindWidget))
  class UCreditsWidget* CreditsWidget;

  UPROPERTY(meta = (BindWidget))
  class UButton* ContinueButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* NewGameButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* LoadMenuButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* SettingsButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* CreditsButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* ExitButton;

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* StartupAnim;
  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* ShowPageAnim;
  // UPROPERTY(Transient, meta = (BindWidgetAnim))
  // class UWidgetAnimation* HidePageAnim;

  UPROPERTY(EditAnywhere)
  class USoundBase* StartupSound;
  UPROPERTY(EditAnywhere)
  class USoundBase* HoverSound;
  UPROPERTY(EditAnywhere)
  class USoundBase* SelectSound;

  UPROPERTY(EditAnywhere)
  class AMainMenuControlHUD* MainMenuControlHUD;

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
  void Continue();
  UFUNCTION()
  void NewGame();
  UFUNCTION()
  void LoadMenu();
  UFUNCTION()
  void SettingsMenu();
  UFUNCTION()
  void Credits();
  UFUNCTION()
  void Exit();

  void ShowSplashScreen();

  void RefreshUI();
  void InitUI(AMainMenuControlHUD* _MainMenuControlHUD,
              FInUIInputActions _InUIInputActions,
              class ASettingsManager* _SettingsManager,
              class ASaveManager* _SaveManager);

  // * Pause state currently doesn't have inputs.
  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();
};