// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "PickupComponent.generated.h"

UENUM()
enum class EPickupType : uint8 {
  Fixed UMETA(DisplayName = "Fixed"),
  Spawned UMETA(DisplayName = "Spawned"),
  Random UMETA(DisplayName = "Random"),  // Note: Not used.
};
UENUM()
enum class EPickupGoodType : uint8 {
  Item UMETA(DisplayName = "Item"),
  Money UMETA(DisplayName = "Money"),
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UPickupComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UPickupComponent();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere)
  EPickupType PickupType;
  UPROPERTY(EditAnywhere)
  EPickupGoodType PickupGoodType;

  UPROPERTY(EditAnywhere)
  class USoundBase* SpawnSound;

  UPROPERTY(EditAnywhere)
  TArray<FDialogueData> DialogueArray;

  UPROPERTY(EditAnywhere)
  FName ItemID;
  UPROPERTY(EditAnywhere)
  int32 ItemQuantity;

  UPROPERTY(EditAnywhere)
  float MoneyAmount;

  UPROPERTY(EditAnywhere, SaveGame)
  bool bIsPicked;  // * To save the state of fixed pickups.

  void DestroyPickup();
  void InitPickup(EPickupGoodType _PickupGoodType, float _MoneyAmount, FName _ItemID, float ItemValue = 0.0f);
};