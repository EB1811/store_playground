// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuControlHUD.generated.h"

UCLASS()
class STORE_PLAYGROUND_API AMainMenuControlHUD : public AHUD {
  GENERATED_BODY()

public:
  AMainMenuControlHUD();
  virtual void DrawHUD() override;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> MainMenuWidgetClass;
  UPROPERTY()
  class UMainMenuWidget* MainMenuWidget;

  UPROPERTY()
  class USettingsSaveGame* CurrentSettingsSaveGame;

  void CreateNewSaveGame();
  void SaveSettings();
  void LoadSettings();

  UFUNCTION(BlueprintCallable, Category = "Widgets")
  void StartNewGame();
  UFUNCTION(BlueprintCallable, Category = "Widgets")
  void ContinueGame();
};