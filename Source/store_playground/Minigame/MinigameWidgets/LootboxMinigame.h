// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Minigame/MiniGameStructs.h"
#include "store_playground/UI/UIStructs.h"
#include "LootboxMinigame.generated.h"

USTRUCT()
struct FLootboxMinigameParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  float BaseWinChance;

  UPROPERTY(EditAnywhere)
  float BaseLootboxPriceMin;
  UPROPERTY(EditAnywhere)
  float BaseLootboxPriceMax;

  UPROPERTY(EditAnywhere)
  int32 MaxItems;
};

// ? Change to actor (with attached widget) for more flexibility (tick, etc.).
UCLASS()
class STORE_PLAYGROUND_API ULootboxMinigame : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(EditAnywhere)
  TSubclassOf<class APickupActor> PickupActorClass;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* PriceText;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ErrorText;
  UPROPERTY(meta = (BindWidget))
  class UButton* OpenLootboxButton;
  UPROPERTY(meta = (BindWidget))
  class UCommonActionWidget* OpenActionWidget;
  UPROPERTY(meta = (BindWidget))
  class UButton* BackButton;

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* ShowAnim;
  UPROPERTY(Transient, meta = (BindWidgetAnim))
  class UWidgetAnimation* HideAnim;
  UPROPERTY(EditAnywhere)
  class USoundBase* OpenSound;
  UPROPERTY(EditAnywhere)
  class USoundBase* HideSound;

  UPROPERTY(EditAnywhere)
  class USoundBase* OpenBoxSound;

  UPROPERTY(EditAnywhere)
  FLootboxMinigameParams LootboxMinigameParams;

  UPROPERTY(EditAnywhere)
  const class AMarket* Market;

  UPROPERTY(EditAnywhere)
  class UMiniGameComponent* MiniGameC;
  UPROPERTY(EditAnywhere)
  class AStore* Store;
  UPROPERTY(EditAnywhere)
  class UInventoryComponent* PlayerInventory;

  UPROPERTY(EditAnywhere)
  TMap<FName, float> PersistentData;

  UPROPERTY(EditAnywhere)
  float Price;

  UFUNCTION()
  void Open();
  UFUNCTION()
  void Back();

  std::function<void(TMap<FName, float>)> UpdatePersistentDataFunc;

  void InitUI(FInUIInputActions InUIInputActions,
              const class AMarket* Market,
              class UMiniGameComponent* _MiniGameC,
              class AStore* Store,
              class UInventoryComponent* PlayerInv,
              TMap<FName, float> PersistentData,
              std::function<void(TMap<FName, float>)> UpdatePersistentDataFunc,
              std::function<void(class UUserWidget*)> _CloseWidgetFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  UPROPERTY(EditAnywhere)
  FUIBehaviour UIBehaviour;
  void SetupUIBehaviour();

  std::function<void(class UUserWidget*)> CloseWidgetFunc;
};