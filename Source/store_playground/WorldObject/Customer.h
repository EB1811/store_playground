// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Customer.generated.h"

UCLASS()
class STORE_PLAYGROUND_API ACustomer : public APaperZDCharacter {
  GENERATED_BODY()

public:
  ACustomer();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "Customer")
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere, Category = "Customer")
  class UDialogueComponent* DialogueComponent;

  UPROPERTY(EditAnywhere, Category = "Customer")
  class UQuestComponent* QuestComponent;

  UPROPERTY(EditAnywhere, Category = "Customer")
  class UCustomerAIComponent* CustomerAIComponent;

  UPROPERTY(EditAnywhere, Category = "Customer")
  class UWidgetComponent* WidgetComponent;
};