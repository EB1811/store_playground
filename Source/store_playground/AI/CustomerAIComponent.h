// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Dialogue/DialogueDataStructs.h"
#include "store_playground/Item/ItemDataStructs.h"
#include "CustomerAIComponent.generated.h"

UENUM()
enum class ECustomerState : uint8 {
  None UMETA(DisplayName = "NONE"),
  Browsing UMETA(DisplayName = "Browsing"),
  Requesting UMETA(DisplayName = "Requesting"),
  Negotiating UMETA(DisplayName = "Negotiating"),
  Leaving UMETA(DisplayName = "Leaving"),
};

UENUM()
enum class ECustomerType : uint8 { Unique, Generic };

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UCustomerAIComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UCustomerAIComponent();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Decision AI")
  ECustomerType CustomerType;

  UPROPERTY(EditAnywhere, Category = "Decision AI")
  TArray<EItemEconType> ItemEconTypes;
  UPROPERTY(EditAnywhere, Category = "Decision AI")
  float MoneyToSpend;
  UPROPERTY(EditAnywhere, Category = "Decision AI")
  ECustomerAttitude Attitude;

  UPROPERTY(EditAnywhere, Category = "Decision AI")
  TArray<FName> WantedBaseItemIDs;

  UPROPERTY(EditAnywhere, Category = "Decision AI")
  class UNegotiationAI* NegotiationAI;

  // Temp: No actions for now, directly change state.
  UPROPERTY(EditAnywhere, Category = "Decision AI")
  ECustomerState CustomerState;

  void StartNegotiation();
  void PostNegotiation();
};