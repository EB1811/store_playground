// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Player/PlayerZDCharacter.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "StorePhaseManager.generated.h"

UENUM()
enum class EStorePhaseState : uint8 {
  None UMETA(DisplayName = "NONE"),
  Morning UMETA(DisplayName = "Morning"),
  MorningBuildMode UMETA(DisplayName = "Morning"),  // * Toggle for when the player wants to build.
  ShopOpen UMETA(DisplayName = "Shop Open"),
  Night UMETA(DisplayName = "Night"),
};
inline auto GetStorePhaseText(EStorePhaseState PhaseState) -> FText {
  switch (PhaseState) {
    case EStorePhaseState::Morning: return FText::FromString("Morning");
    case EStorePhaseState::MorningBuildMode: return FText::FromString("Morning");
    case EStorePhaseState::ShopOpen: return FText::FromString("Shop Open");
    case EStorePhaseState::Night: return FText::FromString("Night");
    default: return FText::FromString("");
  }
}
UENUM()
enum class EStorePhaseAction : uint8 {
  Start UMETA(DisplayName = "Start"),
  OpenShop UMETA(DisplayName = "Open Shop"),
  ToggleBuildMode UMETA(DisplayName = "Toggle Build Mode"),
  CloseShop UMETA(DisplayName = "Close Shop"),
  EndDay UMETA(DisplayName = "End Day"),
};
EStorePhaseState GetNextStorePhaseState(EStorePhaseState CurrentState, EStorePhaseAction Action);

USTRUCT()
struct FStorePhaseManagerParams {
  GENERATED_BODY()
};

USTRUCT()
struct FStorePhaseBehaviorParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  float OpenShopDuration;

  UPROPERTY(EditAnywhere, SaveGame)
  bool bCanAdjustStockWhileOpen;  // ? Put into upgrade manager?
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AStorePhaseManager : public AInfo {
  GENERATED_BODY()

public:
  AStorePhaseManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class TSubclassOf<class AActor> BuildableClass;

  UPROPERTY(EditAnywhere)
  class USoundBase* NextPhaseSound;

  UPROPERTY(EditAnywhere, Category = "Store Phase")
  FStorePhaseManagerParams StorePhaseManagerParams;

  UPROPERTY(EditAnywhere, Category = "Store Phase", SaveGame)
  FStorePhaseBehaviorParams BehaviorParams;

  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class APlayerCommand* PlayerCommand;
  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class ASaveManager* SaveManager;
  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class ADayManager* DayManager;
  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class AMusicManager* MusicManager;
  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class AStore* Store;
  UPROPERTY(EditAnywhere, Category = "Store Phase")
  class ACustomerAIManager* CustomerAIManager;
  UPROPERTY(EditAnywhere, Category = "DayManager")
  class AUpgradeManager* UpgradeManager;

  UPROPERTY(EditAnywhere, Category = "Store Phase", SaveGame)
  EStorePhaseState StorePhaseState;

  FTimerHandle OpenShopTimerHandle;
  UFUNCTION()
  void OnOpenShopTimerEnd();

  void ShopOpenConsiderPlayerState(EPlayerState PlayerBehaviourState);

  void Start();
  void ToggleBuildMode();
  void OpenShop();
  void CloseShop();
  void EndDay();
  void NextPhase();
  void EnterBuildMode();

  UPROPERTY(EditAnywhere, Category = "Store Phase")
  FUpgradeable Upgradeable;
  void ChangeBehaviorParam(const TMap<FName, float>& ParamValues);
};