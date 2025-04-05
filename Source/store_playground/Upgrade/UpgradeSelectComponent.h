// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Upgrade/UpgradeStructs.h"
#include "UpgradeSelectComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UUpgradeSelectComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UUpgradeSelectComponent() {
    PrimaryComponentTick.bCanEverTick = false;
    UpgradeClass = EUpgradeClass::Artisanal;
  }

  UPROPERTY(EditAnywhere, Category = "UpgradeSelectComponent")
  EUpgradeClass UpgradeClass;

  UPROPERTY(EditAnywhere, Category = "UpgradeSelectComponent")
  FText Title;
  UPROPERTY(EditAnywhere, Category = "UpgradeSelectComponent")
  FText Description;
};