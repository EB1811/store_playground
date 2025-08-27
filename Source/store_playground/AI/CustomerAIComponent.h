// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "store_playground/Npc/NpcDataStructs.h"
#include "store_playground/AI/AIStructs.h"
#include "CustomerAIComponent.generated.h"

UENUM()
enum class ECustomerState : uint8 {
  None UMETA(DisplayName = "NONE"),
  Browsing UMETA(DisplayName = "Browsing"),
  BrowsingTalking UMETA(DisplayName = "Browsing Talking"),
  Requesting UMETA(DisplayName = "Requesting"),
  Negotiating UMETA(DisplayName = "Negotiating"),
  Leaving UMETA(DisplayName = "Leaving"),
  RequestingQuest UMETA(DisplayName = "Requesting Quest"),
  PerformingQuest UMETA(DisplayName = "Performing Quest"),
};

UENUM()
enum class ECustomerType : uint8 { Unique, Generic };

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UCustomerAIComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UCustomerAIComponent();

  virtual void BeginPlay() override;
  virtual void TickComponent(float DeltaTime,
                             ELevelTick TickType,
                             FActorComponentTickFunction* ThisTickFunction) override;

  UPROPERTY(EditAnywhere, Category = "Decision AI")
  FGuid CustomerAIID;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  FText CustomerName;
  UPROPERTY(EditAnywhere, Category = "Decision AI")
  ECustomerType CustomerType;

  UPROPERTY(EditAnywhere, Category = "Decision AI")
  TArray<EItemEconType> ItemEconTypes;
  UPROPERTY(EditAnywhere, Category = "Decision AI")
  float AvailableMoney;
  UPROPERTY(EditAnywhere, Category = "Decision AI")
  ECustomerAttitude Attitude;
  UPROPERTY(EditAnywhere, Category = "Decision AI")
  FGameplayTagContainer Tags;

  UPROPERTY(EditAnywhere, Category = "Decision AI")
  ECustomerAction CustomerAction;
  UPROPERTY(EditAnywhere, Category = "Decision AI")
  TArray<FName> ActionRelevantIDs;  // * Item ids, WantedItemTypeIDs, etc.

  UPROPERTY(EditAnywhere, Category = "Decision AI")
  FNegotiationAIDetails NegotiationAIDetails;

  UPROPERTY(EditAnywhere, Category = "Decision AI")
  ECustomerState CustomerState;

  UPROPERTY(EditAnywhere, Category = "Decision AI")
  float RequestingTime;  // * Time the customer waits.

  void StartDialogue();
  void LeaveDialogue();

  void StartQuest();
  void LeaveQuest();
  void FinishQuest();

  void StartNegotiation();
  void LeaveRequestDialogue();
  void PostNegotiation();
};