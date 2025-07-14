// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "TagsComponent.generated.h"

// * Stores different tags for the component owner to influence and interact with the game systems.
// * For example, quest tags can be used to trigger quest-related events, cutscene tags can be used to trigger cutscenes, etc.

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UTagsComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UTagsComponent() { PrimaryComponentTick.bCanEverTick = false; }

  UPROPERTY(EditAnywhere, SaveGame)
  FGameplayTagContainer ConfigurationTags;
  UPROPERTY(EditAnywhere, SaveGame)
  FGameplayTagContainer QuestTags;
  UPROPERTY(EditAnywhere, SaveGame)
  FGameplayTagContainer CutsceneTags;
};