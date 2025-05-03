// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveSlotsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API USaveSlotsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(EditAnywhere)
  TSubclassOf<class UPopulatedSaveSlotWidget> PopulatedSaveSlotWidgetClass;
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UEmptySaveSlotWidget> EmptySaveSlotWidgetClass;

  UPROPERTY()
  class ASaveManager* SaveManagerRef;

  UPROPERTY(meta = (BindWidget))
  class UWrapBox* SaveSlotsWrapBox;
  UPROPERTY(meta = (BindWidget))
  class UButton* ExitMenuButton;

  // TODO: Use load slots widget instead.
  UPROPERTY(EditAnywhere)
  bool bIsSaving;  // * True if saving, false if loading.

  void RefreshUI();

  UFUNCTION()
  void OnExitMenuButtonClicked();

  std::function<void()> CloseSaveSlotsUI;
};