// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Minigame/MiniGameStructs.h"
#include "store_playground/Player/InputStructs.h"
#include "MiniGameManager.generated.h"

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AMiniGameManager : public AInfo {
  GENERATED_BODY()

public:
  AMiniGameManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere) TSubclassOf<class ULootboxMinigame> LootboxMinigameClass;
  UPROPERTY(EditAnywhere)
  class ULootboxMinigame* LootboxMinigameWidget;

  UPROPERTY(EditAnywhere)
  TMap<EMiniGame, FMiniGameInfo> MiniGameInfoMap;

  UPROPERTY(EditAnywhere)
  const class AMarket* Market;
  UPROPERTY(EditAnywhere)
  const class AMarketEconomy* MarketEconomy;

  UPROPERTY(EditAnywhere)
  class AStore* Store;

  UPROPERTY(EditAnywhere, SaveGame)
  TMap<FName, float> PersistentData;  // * Any persistent data for mini-games for stateful mini-games.

  auto GetMiniGameWidget(FInUIInputActions InUIInputActions,
                         class UMiniGameComponent* MiniGameC,
                         class UInventoryComponent* PlayerInventory,
                         std::function<void(class UUserWidget*)> CloseWidgetFunc) -> class UUserWidget*;
};