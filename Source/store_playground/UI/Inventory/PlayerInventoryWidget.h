// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInventoryWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UPlayerInventoryWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;
  virtual void NativeConstruct() override;

  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class UInventoryWidget* InventoryWidget;
};