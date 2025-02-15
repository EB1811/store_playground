// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemDragDropOp.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UItemDragDropOp : public UDragDropOperation
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Drag Drop")
  class UItemBase* SourceItem;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Drag Drop")
  class UInventoryComponent* SourceInventory;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Drag Drop")
  class UInventoryWidget* SourceInventoryWidget;
};