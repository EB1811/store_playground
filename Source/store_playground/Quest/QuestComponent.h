// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Npc/NpcDataStructs.h"
#include "QuestComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UQuestComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UQuestComponent();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Quest Data")
  FQuestChainData QuestChainData;
};