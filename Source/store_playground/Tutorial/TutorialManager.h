// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "store_playground/Tutorial/TutorialStructs.h"
#include "GameFramework/Info.h"
#include "TutorialManager.generated.h"

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ATutorialManager : public AInfo {
  GENERATED_BODY()

public:
  ATutorialManager() { PrimaryActorTick.bCanEverTick = false; }

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere)
  const class ASettingsManager* SettingsManager;

  UPROPERTY(EditAnywhere)
  class UTagsComponent* PlayerTags;
  UPROPERTY(EditAnywhere)
  class APlayerCommand* PlayerCommand;

  UPROPERTY(EditAnywhere)
  TArray<FUITutorialStep> TutorialSteps;
  UPROPERTY(EditAnywhere)
  TMap<FGameplayTag, FUITutorialData> TutorialsData;  // * IdTag -> data.

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FName> CompletedTutorials;

  auto CheckAndShowTutorial(FGameplayTag TutorialIdTag) -> bool;
};
