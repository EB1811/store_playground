// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/Minigame/MiniGameStructs.h"
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
};

// ? Change to actor (with attached widget) for more flexibility (tick, etc.).
UCLASS()
class STORE_PLAYGROUND_API ULootboxMinigame : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* PriceText;
  UPROPERTY(meta = (BindWidget))
  class UButton* OpenLootboxButton;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ResultText;
  UPROPERTY(meta = (BindWidget))
  class UImage* ItemIcon;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* ItemGivenText;

  UPROPERTY(EditAnywhere)
  const class AMarket* Market;

  UPROPERTY(EditAnywhere)
  FLootboxMinigameParams LootboxMinigameParams;

  UPROPERTY(EditAnywhere)
  class AStore* Store;
  UPROPERTY(EditAnywhere)
  class UInventoryComponent* PlayerInventory;
  UPROPERTY(EditAnywhere)
  TMap<FName, float> PersistentData;

  UPROPERTY(EditAnywhere)
  float Price;

  UFUNCTION(BlueprintCallable)
  void OnOpenLootboxButtonClicked();

  std::function<void(TMap<FName, float>)> UpdatePersistentDataFunc;
  std::function<void(class UUserWidget*)> CloseMinigameFunc;

  void InitMiniGame(const class AMarket* Market,
                    class AStore* Store,
                    class UInventoryComponent* PlayerInv,
                    TMap<FName, float> PersistentData,
                    std::function<void(TMap<FName, float>)> UpdatePersistentDataFunc,
                    std::function<void(class UUserWidget*)> _CloseMinigameFunc);
};