// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIStructs.generated.h"

// * Common UI actions.
USTRUCT()
struct FUIActionable {
  GENERATED_BODY()

  std::function<void()> AdvanceUI;
};