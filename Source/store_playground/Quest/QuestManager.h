// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/NPC/NpcDataStructs.h"
#include "QuestManager.generated.h"

USTRUCT()
struct FQuestInProgressData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  TArray<FName> ChainCompletedIDs;
  UPROPERTY(EditAnywhere)
  TArray<FName> ChoicesMade;
  UPROPERTY(EditAnywhere)
  TMap<FName, bool> NegotiationOutcomesMap;
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AQuestManager : public AInfo {
  GENERATED_BODY()

public:
  AQuestManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "QuestManager | Data")
  const class AGlobalDataManager* GlobalDataManager;

  UPROPERTY(EditAnywhere, Category = "Market")
  class TSubclassOf<class ANpc> NpcClass;

  UPROPERTY(EditAnywhere, Category = "QuestManager | Tracking")
  TArray<FName> QuestsCompleted;
  UPROPERTY(EditAnywhere, Category = "QuestManager | Tracking")
  TMap<FName, FQuestInProgressData> QuestInProgressMap;

  TArray<struct FQuestChainData> GetEligibleQuestChains(const TArray<FName>& QuestIDs) const;
  void CompleteQuestChain(const FQuestChainData& QuestChainData,
                          TArray<FName> MadeChoiceIds = {},
                          bool bNegotiationSuccess = false);
};