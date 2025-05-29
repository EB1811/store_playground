// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputStructs.generated.h"

USTRUCT()
struct FInputActions {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* MoveAction;

  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* OpenPauseMenuAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* CloseTopOpenMenuAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* CloseAllMenusAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* OpenInventoryViewAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* BuildModeAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* InteractAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* OpenNewspaperAction;

  // Will probably turn into a combined "store" view action.
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* OpenStatisticsAction;
  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* OpenStoreExpansionsAction;

  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* AdvanceUIAction;  // * Advance UI (dialogue, negotiation, etc.).

  UPROPERTY(EditAnywhere, Category = "Input")
  UInputAction* SkipCutsceneAction;  // * Skip cutscene.
};