// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UInventoryWidget : public UUserWidget
{
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;
  // virtual bool NativeOnDrop(const FGeometry& InGeometry,
  //                           const FDragDropEvent& InDragDropEvent,
  //                           UDragDropOperation* InOperation) override;

  UPROPERTY(meta = (BindWidget))
  class UWrapBox* InventoryPanelWrapBox;
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UInventoryItemSlotWidget> ItemSlotClass;

  UPROPERTY(EditAnywhere)
  class UInventoryComponent* InventoryRef;

  // void SetInventoryRef(class UInventoryComponent* InventoryComponent);
  void RefreshInventory();
};