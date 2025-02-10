// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SPGHUD.generated.h"

// todo Expand as needed.
UENUM()
enum class EHUDState : uint8 {
  HS_IN_GAME UMETA(DisplayName = "InGame"),
  HS_PAUSED UMETA(DisplayName = "Paused"),
  HS_MAINMENU UMETA(DisplayName = "MainMenu")
};

UCLASS()
class STORE_PLAYGROUND_API ASpgHUD : public AHUD
{
  GENERATED_BODY()

public:
  ASpgHUD();
  virtual void DrawHUD() override;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> MainMenuWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> ContainerWidgetClass;

  // UPROPERTY()
  // class UMainMenu* MainMenu;

  UPROPERTY()
  EHUDState HUDState;

  // UFUNCTION(BlueprintCallable, Category = "Widgets")
  // void OpenMainMenu();

  UPROPERTY()
  class UInventoryWidget* ItemContainerWidget;
  void SetAndOpenContainer(const class UInventoryComponent* InventoryComponent);
  UFUNCTION(BlueprintCallable, Category = "Widgets")
  void CloseContainer();
};