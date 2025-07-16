// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "Animation/WidgetAnimation.h"
#include "CoreMinimal.h"
#include "store_playground/Player/InputStructs.h"
#include "UIStructs.generated.h"

// * Common UI actions.
USTRUCT()
struct FUIActionable {
  GENERATED_BODY()

  std::function<void()> AdvanceUI;
  std::function<void()> RetractUI;
  std::function<void()> QuitUI;

  std::function<void(float)> NumericInput;
  std::function<void(FVector2D)> DirectionalInput;

  // * Side buttons for various actions, e.g., sorting, filtering, etc.
  std::function<void()> SideButton1;
  std::function<void()> SideButton2;
  std::function<void()> SideButton3;
  std::function<void()> SideButton4;

  // * Cycle buttons.
  std::function<void()> CycleLeft;
  std::function<void()> CycleRight;
};

// * UI Flavor like animations and sounds.
USTRUCT()
struct FUIBehaviour {
  GENERATED_BODY()

  // * Attempt to pass the animnation itself.
  UWidgetAnimation* ShowAnim;
  UWidgetAnimation* HideAnim;
  USoundBase* OpenSound;
  USoundBase* HideSound;
};