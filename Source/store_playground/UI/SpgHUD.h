// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <stack>
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
class STORE_PLAYGROUND_API ASpgHUD : public AHUD {
  GENERATED_BODY()

public:
  ASpgHUD();
  virtual void DrawHUD() override;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> MainMenuWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> PlayerInventoryWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> PlayerAndContainerWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> UNegotiationWidgetClass;

  // UPROPERTY()
  // class UMainMenu* MainMenu;
  // UFUNCTION(BlueprintCallable, Category = "Widgets")
  // void OpenMainMenu();

  UPROPERTY() EHUDState HUDState;

  UPROPERTY(EditAnywhere, Category = "Widgets")
  TArray<class UUserWidget*> OpenedWidgets;
  UFUNCTION(BlueprintCallable, Category = "Widgets")
  void CloseTopOpenMenu();
  UFUNCTION(BlueprintCallable, Category = "Widgets")
  void CloseAllMenus();

  void CloseWidget(class UUserWidget* Widget);

  UPROPERTY()
  class UPlayerInventoryWidget* PlayerInventoryWidget;
  void SetAndOpenInventory(const class UInventoryComponent* Inventory);

  UPROPERTY()
  class UPlayerAndContainerWidget* PlayerAndContainerWidget;
  void SetAndOpenContainer(const class UInventoryComponent* PlayerInventory,
                           const class UInventoryComponent* ContainerInventory);

  UPROPERTY()
  class UNegotiationWidget* NegotiationWidget;
  void SetAndOpenNegotiation(const class UNegotiationSystem* Negotiation);
};