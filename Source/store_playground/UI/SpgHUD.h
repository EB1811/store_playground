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
  TSubclassOf<class UUserWidget> InventoryViewWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> BuildableDisplayWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> StockDisplayWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> PlayerAndContainerWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> NpcStoreWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> UDialogueWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> UNegotiationWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> NewspaperWidgetClass;
  UPROPERTY(EditAnywhere, Category = "Widgets")
  TSubclassOf<class UUserWidget> UpgradeSelectWidgetClass;

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
  class UInventoryViewWidget* InventoryViewWidget;
  void SetAndOpenInventoryView(class UInventoryComponent* PlayerInventory, class AStore* Store);

  UPROPERTY()
  class UBuildableDisplayWidget* BuildableDisplayWidget;
  void SetAndOpenBuildableDisplay(class ABuildable* Buildable,
                                  std::function<bool(class ABuildable* Buildable)> BuildStockDisplayFunc,
                                  std::function<bool(class ABuildable* Buildable)> BuildDecorationFunc);

  UPROPERTY()
  class UStockDisplayWidget* StockDisplayWidget;
  void SetAndOpenStockDisplay(class UStockDisplayComponent* StockDisplay,
                              class UInventoryComponent* DisplayInventory,
                              class UInventoryComponent* PlayerInventory,
                              std::function<void(class UItemBase*, class UInventoryComponent*)> PlayerToDisplayFunc,
                              std::function<void(class UItemBase*, class UInventoryComponent*)> DisplayToPlayerFunc);

  UPROPERTY()
  class UPlayerAndContainerWidget* PlayerAndContainerWidget;
  void SetAndOpenContainer(const class UInventoryComponent* PlayerInventory,
                           const class UInventoryComponent* ContainerInventory);

  UPROPERTY()
  class UNpcStoreWidget* NpcStoreWidget;
  void SetAndOpenNPCStore(class UInventoryComponent* NPCStoreInventory,
                          class UInventoryComponent* PlayerInventory,
                          std::function<void(class UItemBase*, class UInventoryComponent*)> PlayerToStoreFunc,
                          std::function<void(class UItemBase*, class UInventoryComponent*)> StoreToPlayerFunc);

  UPROPERTY()
  class UDialogueWidget* DialogueWidget;
  void SetAndOpenDialogue(class UDialogueSystem* Dialogue, std::function<void()> OnDialogueEndFunc = nullptr);

  UPROPERTY()
  class UNegotiationWidget* NegotiationWidget;
  void SetAndOpenNegotiation(const class UNegotiationSystem* Negotiation, class UInventoryComponent* PlayerInventoryC);

  UPROPERTY()
  class UNewspaperWidget* NewspaperWidget;
  void SetAndOpenNewspaper(const class ANewsGen* NewsGenRef);

  UPROPERTY()
  class UUpgradeListWidget* UpgradeSelectWidget;
  void SetAndOpenUpgradeSelect(class UUpgradeSelectComponent* UpgradeSelectC, class AUpgradeManager* UpgradeManager);
};