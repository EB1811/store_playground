// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Customer.generated.h"

UCLASS()
class STORE_PLAYGROUND_API ACustomer : public AActor {
  GENERATED_BODY()

public:
  ACustomer();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Customer")
  UStaticMeshComponent* Mesh;

  UPROPERTY(EditAnywhere, Category = "Customer")
  class UDialogueComponent* DialogueComponent;

  UPROPERTY(EditAnywhere, Category = "Customer")
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere, Category = "Customer")
  class UCustomerAIComponent* CustomerAIComponent;
};