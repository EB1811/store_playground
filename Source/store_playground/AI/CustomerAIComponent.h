// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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
  UPROPERTY(EditAnywhere, Category = "Decision AI")
  class UNegotiationAI* NegotiationAI;
};