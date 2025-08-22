#include "GameSettingsWidget.h"
#include "CoreFwd.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Framework/SettingsManager.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UGameSettingsWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ApplyButton->OnClicked.AddDynamic(this, &UGameSettingsWidget::Apply);
  BackButton->OnClicked.AddDynamic(this, &UGameSettingsWidget::Back);

  SetupUIActionable();
}

void UGameSettingsWidget::Apply() {
  UGameUserSettings* GameSettings = SettingsManager->UnrealSettings;

  FString SelectedDifficulty = DifficultyComboBox->GetSelectedOption();
  bool bShowTutorials = ShowTutorialCheckBox->IsChecked();

  // Temp

  SettingsManager->SaveSettings();
}
void UGameSettingsWidget::Back() { BackFunc(); }

void UGameSettingsWidget::RefreshUI() { UGameUserSettings* GameSettings = SettingsManager->UnrealSettings; }

void UGameSettingsWidget::InitUI(FInUIInputActions _InUIInputActions,
                                 ASettingsManager* _SettingsManager,
                                 std::function<void()> _BackFunc) {
  check(_SettingsManager && _BackFunc);

  SettingsManager = _SettingsManager;
  BackFunc = _BackFunc;

  DifficultyComboBox->ClearOptions();
  // Temp
  DifficultyComboBox->AddOption(TEXT("Easy"));
  DifficultyComboBox->AddOption(TEXT("Normal"));
  DifficultyComboBox->AddOption(TEXT("Hard"));
  DifficultyComboBox->SetSelectedIndex(1);
}

void UGameSettingsWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { Apply(); };
  UIActionable.RetractUI = [this]() { Back(); };
}