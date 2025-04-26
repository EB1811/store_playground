// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/StoreExpansionManager/StoreExpansionManager.h"
#include "StoreExpansionsListWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UStoreExpansionsListWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UStoreExpansionSelectWidget> StoreExpansionSelectWidgetClass;

  UPROPERTY(EditAnywhere)
  const class AStoreExpansionManager* StoreExpansionManagerRef;

  UPROPERTY(meta = (BindWidget))
  class UWrapBox* ExpansionsListPanelWrapBox;

  std::function<void(EStoreExpansionLevel)> SelectExpansionFunc;

  void RefreshUI();
};