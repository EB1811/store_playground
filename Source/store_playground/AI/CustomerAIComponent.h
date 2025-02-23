// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include <vector>
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "CustomerAIComponent.generated.h"

UENUM()
enum class ECustomerType : uint8 {
  Farmer,
  Merchant,
  Noble,
  Peasant,
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UCustomerAIComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UCustomerAIComponent();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Decision AI")
  ECustomerType CustomerType;

  // Temp:  Index of dialogues. Storing here for now. Move to manager.
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  struct FDataTableCategoryHandle FriendlyDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  struct FDataTableCategoryHandle NeutralDialoguesTable;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  struct FDataTableCategoryHandle HostileDialoguesTable;

  // UPROPERTY(EditAnywhere, Category = "Dialogue")
  // TMap<ENegotiationDialogueType, TArray<struct FDialogueData>> FriendlyDialoguesMap;
  // UPROPERTY(EditAnywhere, Category = "Dialogue")
  // TMap<ENegotiationDialogueType, TArray<struct FDialogueData>> NeutralDialoguesMap;
  // UPROPERTY(EditAnywhere, Category = "Dialogue")
  // TMap<ENegotiationDialogueType, TArray<struct FDialogueData>> HostileDialoguesMap;

  UPROPERTY(EditAnywhere, Category = "Decision AI")
  class UNegotiationAI* NegotiationAI;

  void InitializeDialogueData();
};

// Temp: Probably convert to list variables on object when dialogue types are all finalized.
extern std::unordered_map<ENegotiationDialogueType, std::vector<struct FDialogueData>> FriendlyDialoguesMap;
extern std::unordered_map<ENegotiationDialogueType, std::vector<struct FDialogueData>> NeutralDialoguesMap;
extern std::unordered_map<ENegotiationDialogueType, std::vector<struct FDialogueData>> HostileDialoguesMap;