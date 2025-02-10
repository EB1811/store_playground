// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "PlayerZDCharacter.generated.h"

USTRUCT()
struct FInteractionData {
  GENERATED_BODY()

  UPROPERTY()
  float LastInteractionCheckTime;
  UPROPERTY()
  bool IsInteracting;
};

UCLASS()
class STORE_PLAYGROUND_API APlayerZDCharacter : public APaperZDCharacter
{
  GENERATED_BODY()

public:
  APlayerZDCharacter();

  //* Overrides
  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY()
  class ASpgHUD* HUD;

  //* Input
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  class UInputMappingContext* InputMappingContext;
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  class UInputAction* MoveAction;
  UPROPERTY(EditAnywhere, Category = "Character | Input")
  class UInputAction* InteractAction;
  UFUNCTION(BlueprintCallable, Category = "Character | Input")
  void Move(const FInputActionValue& Value);

  //* Inventory
  UPROPERTY(EditAnywhere, Category = "Character | Inventory")
  class UInventoryComponent* InventoryComponent;

  //* Interaction
  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  float InteractionCheckFrequency;
  UPROPERTY(EditAnywhere, Category = "Character | Interaction")
  float InteractionCheckDistance;
  void Interact(const FInputActionValue& Value);
};
