// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Npc/NpcDataStructs.h"
#include "GlobalDataManager.generated.h"

// * Global data store to hold data used by random generation systems.
// * Data comes from the data tables.
// * Actor due to the need to initialize the data and possible tick to update it / check for changes.

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API AGlobalDataManager : public AInfo {
  GENERATED_BODY()

public:
  AGlobalDataManager();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> UniqueNpcDataTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> UniqueNpcDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  TObjectPtr<const class UDataTable> CustomerDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  struct FDataTableCategoryHandle FriendlyNegDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  struct FDataTableCategoryHandle NeutralNegDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Data")
  struct FDataTableCategoryHandle HostileNegDialoguesTable;

  UPROPERTY(EditAnywhere, Category = "NPC")
  TArray<struct FUniqueNpcData> UniqueNpcArray;

  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TMap<FName, FDialoguesArray> UniqueNpcDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TArray<struct FDialogueData> CustomerDialogues;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TMap<ENegotiationDialogueType, FDialoguesArray> FriendlyDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TMap<ENegotiationDialogueType, FDialoguesArray> NeutralDialoguesMap;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TMap<ENegotiationDialogueType, FDialoguesArray> HostileDialoguesMap;

  void InitializeNPCData();
  void InitializeDialogueData();

  struct FUniqueNpcData GetRandomUniqueNpcData() const;

  TArray<struct FDialogueData> GetRandomCustomerDialogue() const;
  TMap<ENegotiationDialogueType, FDialoguesArray> GetRandomNegDialogueMap(
      ECustomerAttitude Attitude = ECustomerAttitude::Neutral) const;
};