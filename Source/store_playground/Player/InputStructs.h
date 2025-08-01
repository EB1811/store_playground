// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputStructs.generated.h"

USTRUCT()
struct FInGameInputActions {
  GENERATED_BODY()

  // * In Game Actions.
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* MoveAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* OpenPauseMenuAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* OpenInventoryViewAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* BuildModeAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* InteractAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* OpenNewspaperAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* OpenStoreViewAction;
};

USTRUCT()
struct FInUIInputActions {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* AdvanceUIAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* RetractUIAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* QuitUIAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* UINumericInputAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* UIDirectionalInputAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* UISideButton1Action;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* UISideButton2Action;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* UISideButton3Action;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* UISideButton4Action;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* UICycleLeftAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* UICycleRightAction;
};

USTRUCT()
struct FInCutsceneInputActions {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* AdvanceCutsceneAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* SkipCutsceneAction;
};