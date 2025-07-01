// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "store_playground/Player/InputStructs.h"
#include "UIStructs.generated.h"

// * Common UI actions.
USTRUCT()
struct FUIActionable {
  GENERATED_BODY()

  // ? Add quit widget action?
  std::function<void()> AdvanceUI;
};

// * UI Flavor like animations and sounds.
USTRUCT()
struct FUIBehaviour {
  GENERATED_BODY()

  std::function<void()> ShowUI;
  std::function<void()> HideUI;
};