// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "store_playground/Item/ItemBase.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FUIOnInventoryUpdated);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STORE_PLAYGROUND_API UInventoryComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  UInventoryComponent();

  virtual void BeginPlay() override;

  FUIOnInventoryUpdated UIOnInventoryUpdated;

  UPROPERTY(EditAnywhere, Category = "Inventory")
  TArray<TObjectPtr<UItemBase>> ItemsArray;
  UPROPERTY(EditAnywhere, Category = "Inventory")
  TArray<FDataTableRowHandle> InitItemIds;

  void AddItem(UItemBase* Item, int32 Amount = 1);
  void RemoveItem(UItemBase* Item, int32 Amount = 1);
};