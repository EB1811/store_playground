// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "ControlsRebindWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UControlsRebindWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* DisplayText;
  UPROPERTY(meta = (BindWidget))
  class UInputKeySelector* RebindKeySelector;
  UPROPERTY(meta = (BindWidget))
  class UButton* ResetButton;

  UPROPERTY(EditAnywhere)
  FLinearColor NormalColor;
  UPROPERTY(EditAnywhere)
  FLinearColor IsRebindingColor;

  UPROPERTY(EditAnywhere)
  FPlayerKeyMapping KeyMapping;

  UFUNCTION()
  void IsSelecting();
  UFUNCTION()
  void Rebind(FInputChord SelectedKey);
  UFUNCTION()
  void Reset();

  void InitUI(FPlayerKeyMapping _KeyMapping,
              std::function<void(FPlayerKeyMapping, FInputChord)> _RebindFunc,
              std::function<void(FPlayerKeyMapping)> _ResetFunc);

  TFunction<void(FPlayerKeyMapping, FInputChord)> RebindFunc;
  TFunction<void(FPlayerKeyMapping)> ResetFunc;
};