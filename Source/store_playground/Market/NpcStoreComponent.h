// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Market/MarketDataStructs.h"
#include "NpcStoreComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UNpcStoreComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UNpcStoreComponent();

  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, Category = "NPC Store")
  FGuid NpcStoreCID;

  UPROPERTY(EditAnywhere, Category = "NPC Store")
  bool bFixedStoreType;  // * If true, the store type will not be randomly generated.

  UPROPERTY(EditAnywhere, Category = "NPC Store")
  FNpcStoreType NpcStoreType;
};