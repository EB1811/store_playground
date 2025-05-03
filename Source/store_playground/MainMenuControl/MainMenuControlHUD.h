// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuControlHUD.generated.h"

USTRUCT()
struct FMainMenuControlParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FString SaveSlotListSaveName;
  UPROPERTY(EditAnywhere)
  int32 SaveSlotCount;
  UPROPERTY(EditAnywhere)
  FString SaveSlotNamePrefix;
  UPROPERTY(EditAnywhere)
  FString AutoSaveSlotName;
};

UCLASS()
class STORE_PLAYGROUND_API AMainMenuControlHUD : public AHUD {
  GENERATED_BODY()

public:
  AMainMenuControlHUD();
  virtual void DrawHUD() override;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  FMainMenuControlParams MainMenuControlParams;

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> MainMenuWidgetClass;
  UPROPERTY()
  class UMainMenuWidget* MainMenuWidget;

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class ULoadSlotsWidget> LoadSlotsWidgetClass;
  UPROPERTY()
  class ULoadSlotsWidget* LoadSlotsWidget;

  UPROPERTY()
  class USettingsSaveGame* CurrentSettingsSaveGame;
  UPROPERTY()
  class USaveSlotListSaveGame* SaveSlotListSaveGame;

  UFUNCTION(BlueprintCallable, Category = "Widgets")
  void StartNewGame();
  UFUNCTION(BlueprintCallable, Category = "Widgets")
  void ContinueGame();
  UFUNCTION(BlueprintCallable, Category = "Widgets")
  void LoadGame();
  UFUNCTION(BlueprintCallable, Category = "Widgets")
  void ExitLoadGameMenu();
  // UFUNCTION(BlueprintCallable, Category = "Widgets")
  // void QuitGame();

  void CreateNewSettingsSaveGame();
  void SaveSettings();
  void LoadSettings();

  // ? Put in a save slot manager?
  void LoadSaveGameSlots();
  void LoadFromSaveSlot(int32 SlotIndex);

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class ULevelLoadingTransitionWidget> LevelLoadingTransitionWidgetClass;
  UPROPERTY()
  class ULevelLoadingTransitionWidget* LevelLoadingTransitionWidget;
};