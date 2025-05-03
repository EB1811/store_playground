// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadSlotsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API ULoadSlotsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UPopulatedSaveSlotWidget> PopulatedSaveSlotWidgetClass;
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UEmptySaveSlotWidget> EmptySaveSlotWidgetClass;

  UPROPERTY()
  class USaveSlotListSaveGame* SaveSlotListSaveGame;
  UPROPERTY()
  int32 SlotCount;

  UPROPERTY(meta = (BindWidget))
  class UWrapBox* SaveSlotsWrapBox;
  UPROPERTY(meta = (BindWidget))
  class UButton* ExitLoadGameMenuButton;

  void RefreshUI();

  std::function<void(int32)> SelectSlotFunc;  // * Function to call when a slot is selected.
};