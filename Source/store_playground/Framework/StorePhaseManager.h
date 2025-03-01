// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "StorePhaseManager.generated.h"

UENUM()
enum class EShopPhaseState : uint8 {
  None UMETA(DisplayName = "NONE"),
  Morning UMETA(DisplayName = "Morning"),
  ShopOpen UMETA(DisplayName = "Shop Open"),
  Night UMETA(DisplayName = "Night"),
};
UENUM()
enum class EShopPhaseAction : uint8 {
  Start UMETA(DisplayName = "Start"),
  OpenShop UMETA(DisplayName = "Open Shop"),
  CloseShop UMETA(DisplayName = "Close Shop"),
  EndDay UMETA(DisplayName = "End Day"),
};

EShopPhaseState GetNextShopPhaseState(EShopPhaseState CurrentState, EShopPhaseAction Action);

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AStorePhaseManager : public AInfo {
  GENERATED_BODY()

public:
  AStorePhaseManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Store Phase")
  EShopPhaseState ShopPhaseState;

  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class AStore* Store;
  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class AMarket* Market;
  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class ACustomerAIManager* CustomerAIManager;

  void Start();
  void OpenShop();
  void CloseShop();
  void EndDay();
  void NextPhase();
};