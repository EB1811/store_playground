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
  TSubclassOf<class UDisabledStoreExpansionSelectWidget> DisabledStoreExpansionSelectWidgetClass;

  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* ExpansionsListBox;
  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* LockedExpansionsListBox;

  UPROPERTY(EditAnywhere)
  class USoundBase* SelectSound;
  UPROPERTY(EditAnywhere)
  class USoundBase* UnlockSound;

  UPROPERTY(EditAnywhere)
  class AStore* Store;
  UPROPERTY(EditAnywhere)
  class AStoreExpansionManager* StoreExpansionManager;

  UPROPERTY(EditAnywhere)
  FName SelectedExpansionID;
  UPROPERTY(EditAnywhere)
  class UStoreExpansionSelectWidget* SelectedExpansionCardWidget;

  void SelectExpansion(FName ExpansionID, UStoreExpansionSelectWidget* ExpansionCardWidget);
  void SelectNextExpansion(FVector2D Direction);

  void UnlockExpansion();

  void RefreshUI();
  void InitUI(class AStore* _Store,
              class AStoreExpansionManager* _StoreExpansionManager,
              TFunction<void()> _CloseWidgetFunc);

  TFunction<void()> CloseWidgetFunc;
};