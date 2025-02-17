// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UDialogueComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UDialogueComponent();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TArray<struct FDataTableRowHandle> DialogueDataTableRows;
  UPROPERTY(EditAnywhere, Category = "Dialogue")
  TArray<struct FDialogueData> DialogueArr;

  UFUNCTION(BlueprintCallable, Category = "Dialogue")
  void LoadDialogueData();
};