// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "TutorialStructs.generated.h"

USTRUCT()
struct FUITutorialStep {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;
  UPROPERTY(EditAnywhere)
  FName TutorialID;  // * FUITutorialData.ID

  UPROPERTY(EditAnywhere)
  FText Title;
  UPROPERTY(EditAnywhere, meta = (MultiLine = true))
  FText Body;
  UPROPERTY(EditAnywhere)
  class UTexture2D* Image;
  UPROPERTY(EditAnywhere)
  class UMediaSource* Video;
};
USTRUCT()
struct FUITutorialStepRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;
  UPROPERTY(EditAnywhere)
  FName TutorialID;  // * FUITutorialData.ID

  UPROPERTY(EditAnywhere)
  FText Title;
  UPROPERTY(EditAnywhere, meta = (MultiLine = true))
  FText Body;
  UPROPERTY(EditAnywhere)
  class UTexture2D* Image;
  UPROPERTY(EditAnywhere)
  class UMediaSource* Video;
};

USTRUCT()
struct FUITutorialData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;
  UPROPERTY(EditAnywhere)
  FGameplayTag IdTag;  // * For semantic id.

  UPROPERTY(EditAnywhere)
  FGameplayTagContainer PlayerTags;  // * Tags that need to be present to activate this tutorial.
};
USTRUCT()
struct FUITutorialDataRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ID;
  UPROPERTY(EditAnywhere)
  FGameplayTag IdTag;  // * For semantic id.

  UPROPERTY(EditAnywhere)
  FGameplayTagContainer PlayerTags;  // * Tags that need to be present to activate this tutorial.
};