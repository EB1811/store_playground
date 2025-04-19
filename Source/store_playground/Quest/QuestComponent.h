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

  UPROPERTY(EditAnywhere, SaveGame)
  FName ChainID;

  UPROPERTY(EditAnywhere, SaveGame)
  ECustomerAction CustomerAction;
  UPROPERTY(EditAnywhere, SaveGame)
  EQuestOutcomeType QuestOutcomeType;

  void PostQuest(TArray<struct FDialogueData> PostDialogues = {});
};