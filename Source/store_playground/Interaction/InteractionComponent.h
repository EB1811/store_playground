// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

// temp: move to player ui controller
DECLARE_DELEGATE_OneParam(FUIOnInteract, int16);

UENUM()
enum class EInteractionType : uint8 {
  Use UMETA(DisplayName = "Use"),
  Npc UMETA(DisplayName = "Npc"),
  Pickup UMETA(DisplayName = "Pickup"),
  Container UMETA(DisplayName = "Container")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UInteractionComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  UInteractionComponent() : InteractionType(EInteractionType::Use) {}

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Interaction")
  EInteractionType InteractionType;

  void InteractUse(FUIOnInteract* UIOnInteract = nullptr) const;
  void InteractNpc(FUIOnInteract* UIOnInteract = nullptr) const;
  void InteractPickup(FUIOnInteract* UIOnInteract = nullptr) const;
  class UInventoryComponent* InteractContainer() const;
};

// ? Should there be different components for different types of interactions?