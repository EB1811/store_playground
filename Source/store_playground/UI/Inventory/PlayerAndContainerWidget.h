// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerAndContainerWidget.generated.h"

// * A container represents any state where there is another inventory to interact with,
// * along with the player's inventory,
// * e.g., a chest, a vendor.

UCLASS()
class STORE_PLAYGROUND_API UPlayerAndContainerWidget : public UUserWidget
{
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;
  virtual void NativeConstruct() override;

  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class UInventoryWidget* PlayerInventoryWidget;
  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class UInventoryWidget* ContainerInventoryWidget;
};