// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "CustomerPC.generated.h"

UCLASS()
class STORE_PLAYGROUND_API ACustomerPC : public APaperCharacter {
  GENERATED_BODY()

public:
  ACustomerPC();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "CustomerPC")
  class UInteractionComponent* InteractionComponent;

  UPROPERTY(EditAnywhere, Category = "CustomerPC")
  class UDialogueComponent* DialogueComponent;

  UPROPERTY(EditAnywhere, Category = "CustomerPC")
  class UQuestComponent* QuestComponent;

  UPROPERTY(EditAnywhere, Category = "CustomerPC")
  class UCustomerAIComponent* CustomerAIComponent;

  UPROPERTY(EditAnywhere, Category = "Npc")
  class USimpleSpriteAnimComponent* SimpleSpriteAnimComponent;

  UPROPERTY(EditAnywhere, Category = "CustomerPC")
  class UWidgetComponent* WidgetComponent;
  void ShowWidget();
};