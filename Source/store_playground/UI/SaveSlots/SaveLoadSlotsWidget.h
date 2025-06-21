// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveLoadSlotsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API USaveLoadSlotsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(EditAnywhere)
  TSubclassOf<class UPopulatedSaveSlotWidget> PopulatedSaveSlotWidgetClass;
  UPROPERTY(EditAnywhere)
  TSubclassOf<class UEmptySaveSlotWidget> EmptySaveSlotWidgetClass;

  UPROPERTY(meta = (BindWidget))
  class UTextBlock* TitleText;
  UPROPERTY(meta = (BindWidget))
  class UVerticalBox* SlotsBox;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* SelectButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* DeleteButton;
  UPROPERTY(meta = (BindWidget))
  class UControlMenuButtonWidget* BackButton;
  UPROPERTY(EditAnywhere, meta = (BindWidget))
  class USaveLoadConfirmWidget* SaveLoadConfirmWidget;

  UPROPERTY()
  class ASaveManager* SaveManagerRef;
  UPROPERTY(EditAnywhere)
  bool bIsSaving;
  UPROPERTY(EditAnywhere)
  bool bIsConfirming;

  void ConfirmSaveLoad(int32 SlotIndex);
  void ConfirmDelete(int32 SlotIndex);

  UFUNCTION()
  void Select();
  UFUNCTION()
  void Delete();
  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(bool _bIsSaving, class ASaveManager* _SaveManager, std::function<void()> _CloseWidgetFunc);

  std::function<void()> BackFunc;
};