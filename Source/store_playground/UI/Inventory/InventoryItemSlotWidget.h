// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemSlotWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UInventoryItemSlotWidget : public UUserWidget
{
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;
  virtual void NativeConstruct() override;
  // virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
  // virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
  // virtual void NativeOnDragDetected(const FGeometry& InGeometry,
  //                                   const FPointerEvent& InMouseEvent,
  //                                   UDragDropOperation*& OutOperation) override;
  // virtual bool NativeOnDrop(const FGeometry& InGeometry,
  //                           const FDragDropEvent& InDragDropEvent,
  //                           UDragDropOperation* InOperation) override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot")
  class UItemBase* ItemRef;
  // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
  // class UInventoryPanel* InventoryPanel;

  // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot")
  // TSubclassOf<class UDragItemVisual> DragVisualClass;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot", meta = (BindWidget))
  class UImage* ItemIcon;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot", meta = (BindWidget))
  class UTextBlock* ItemName;
};