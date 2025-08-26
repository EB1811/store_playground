// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "store_playground/Player/InputStructs.h"
#include "store_playground/Player/PlayerZDCharacter.h"
#include "MainMenuPlayer.generated.h"

// * Player for the main menu.
// * Handles input for navigating the main menu.

UCLASS()
class STORE_PLAYGROUND_API AMainMenuPlayer : public APawn {
  GENERATED_BODY()

public:
  AMainMenuPlayer();

  // * Overrides
  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY()
  class AMainMenuControlHUD* HUD;

  // * Input
  UPROPERTY(EditAnywhere)
  TMap<EPlayerState, class UInputMappingContext*> InputContexts;  // * To register input contexts.
  UPROPERTY(EditAnywhere)
  class UInputMappingContext* UIInputContext;
  UPROPERTY(EditAnywhere)
  FInUIInputActions InUIInputActions;
  // * Input functions
  // In UI
  UFUNCTION(BlueprintCallable)
  void AdvanceUI(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void RetractUIAction(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void UIDirectionalInputAction(const FInputActionValue& Value);
  UFUNCTION(BlueprintCallable)
  void UISideButton4Action(const FInputActionValue& Value);
};