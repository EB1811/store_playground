// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "store_playground/Player/InputStructs.h"
#include "MainMenuControlHUD.generated.h"

UCLASS()
class STORE_PLAYGROUND_API AMainMenuControlHUD : public AHUD {
  GENERATED_BODY()

public:
  AMainMenuControlHUD();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UMainMenuWidget> MainMenuWidgetClass;

  UPROPERTY(EditAnywhere)
  FInUIInputActions InUIInputActions;

  UPROPERTY(EditAnywhere)
  class ASettingsManager* SettingsManager;
  UPROPERTY(EditAnywhere)
  class ASaveManager* SaveManager;

  void AdvanceUI();
  void RetractUIAction();
  void UIDirectionalInputAction(FVector2D Direction);
  void UISideButton4Action();

  UPROPERTY(EditAnywhere, Category = "Widgets")
  class UMainMenuWidget* MainMenuWidget;
  void OpenMainMenu();

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UInitLoadTransitionWidget> InitLoadTransitionWidgetClass;
  UPROPERTY()
  class UInitLoadTransitionWidget* InitLoadTransitionWidget;
  void InitMenuFadeOut(std::function<void()> _FadeOutEndFunc);

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class ULevelLoadingTransitionWidget> LevelLoadingTransitionWidgetClass;
  UPROPERTY()
  class ULevelLoadingTransitionWidget* LevelLoadingTransitionWidget;
  void StartLevelLoadingTransition(std::function<void()> _FadeInEndFunc);

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UAnyKeyTransitionWidget> AnyKeyTransitionWidgetClass;
  UPROPERTY()
  class UAnyKeyTransitionWidget* AnyKeyTransitionWidget;
  void OpenAnyKeyTransition(std::function<void()> _FadeOutEndFunc);

  void PlayWidgetAnim(class UUserWidget* Widget, class UWidgetAnimation* Animation);
};