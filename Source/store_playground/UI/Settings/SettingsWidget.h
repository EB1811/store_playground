// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Player/InputStructs.h"
#include "SettingsWidget.generated.h"

UENUM()
enum class ESettingsCategory : uint8 {
  None UMETA(DisplayName = "None"),
  Game UMETA(DisplayName = "Game"),
  Display UMETA(DisplayName = "Display"),
  Graphics UMETA(DisplayName = "Graphics"),
  Sound UMETA(DisplayName = "Sound"),
  Controls UMETA(DisplayName = "Controls"),
};

UCLASS()
class STORE_PLAYGROUND_API USettingsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UOverlay* SettingsOverlay;
  UPROPERTY(meta = (BindWidget))
  class USoundSettingsWidget* SoundSettingsWidget;
  UPROPERTY(meta = (BindWidget))
  class UDisplaySettingsWidget* DisplaySettingsWidget;
  UPROPERTY(meta = (BindWidget))
  class UGraphicsSettingsWidget* GraphicsSettingsWidget;

  UPROPERTY(meta = (BindWidget))
  class UButton* GameSettingsButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* DisplaySettingsButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* GraphicsSettingsButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* SoundSettingsButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* ControlsSettingsButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* BackButton;

  UPROPERTY(EditAnywhere)
  class ASettingsManager* SettingsManager;

  UPROPERTY(EditAnywhere)
  ESettingsCategory CurrentCategory;
  UPROPERTY(EditAnywhere)
  UUserWidget* OpenedWidget;

  UFUNCTION()
  void GameSettings();
  UFUNCTION()
  void DisplaySettings();
  UFUNCTION()
  void GraphicsSettings();
  UFUNCTION()
  void SoundSettings();
  UFUNCTION()
  void ControlsSettings();
  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInUIInputActions _InUIInputActions,
              class ASettingsManager* _SettingsManager,
              std::function<void()> _CloseWidgetFunc);

  std::function<void()> CloseWidgetFunc;
};