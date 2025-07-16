// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
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

  // * Need callbacks since theres no generic way to wait for hide animation finish.
  // * And avoid problems such as being able to exit the UI while the animation is still playing, resulting in a stuck state.
  std::function<void(std::function<void()>)> ShowUI;
  std::function<void(std::function<void()>)> HideUI;

  // * Override the base sounds for opening and closing the UI.
  // * If not set, the default sounds will be used.
  UPROPERTY(EditAnywhere)
  class USoundBase* OpenSoundOverride;
  UPROPERTY(EditAnywhere)
  class USoundBase* CloseSoundOverride;
};