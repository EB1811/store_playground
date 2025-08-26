// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "store_playground/UI/UIStructs.h"
#include "store_playground/Framework/SettingsStructs.h"
#include "GameSettingsWidget.generated.h"

UCLASS()
class STORE_PLAYGROUND_API UGameSettingsWidget : public UUserWidget {
  GENERATED_BODY()

public:
  virtual void NativeOnInitialized() override;

  UPROPERTY(meta = (BindWidget))
  class UComboBoxString* DifficultyComboBox;
  UPROPERTY(meta = (BindWidget))
  class UTextBlock* DifficultyDescriptionText;
  UPROPERTY(meta = (BindWidget))
  class UCheckBox* ShowTutorialCheckBox;

  UPROPERTY(meta = (BindWidget))
  class UButton* ApplyButton;
  UPROPERTY(meta = (BindWidget))
  class UButton* BackButton;

  UPROPERTY(EditAnywhere)
  TMap<EGameDifficulty, FString> DifficultyNamesMap;
  UPROPERTY(EditAnywhere)
  TMap<EGameDifficulty, FString> DifficultyDescriptionsMap;

  UPROPERTY(EditAnywhere)
  class ASettingsManager* SettingsManager;

  UFUNCTION()
  void ChangeDifficulty(FString SelectedItem, ESelectInfo::Type SelectionType);
  UFUNCTION()
  void Apply();
  UFUNCTION()
  void Back();

  void RefreshUI();
  void InitUI(FInUIInputActions _InUIInputActions,
              class ASettingsManager* _SettingsManager,
              std::function<void()> _BackFunc);

  UPROPERTY(EditAnywhere)
  FUIActionable UIActionable;
  void SetupUIActionable();

  std::function<void()> BackFunc;
};