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

  DifficultyComboBox->OnSelectionChanged.AddDynamic(this, &UGameSettingsWidget::ChangeDifficulty);
  ApplyButton->OnClicked.AddDynamic(this, &UGameSettingsWidget::Apply);
  BackButton->OnClicked.AddDynamic(this, &UGameSettingsWidget::Back);

  SetupUIActionable();

  for (auto Type : TEnumRange<EGameDifficulty>())
    check(DifficultyNamesMap.Contains(Type) && DifficultyDescriptionsMap.Contains(Type));
}

void UGameSettingsWidget::ChangeDifficulty(FString SelectedItem, ESelectInfo::Type SelectionType) {
  if (SelectionType == ESelectInfo::Type::Direct || SelectionType == ESelectInfo::Type::OnKeyPress) return;

  for (auto Type : TEnumRange<EGameDifficulty>()) {
    if (DifficultyNamesMap[Type] == SelectedItem) {
      DifficultyDescriptionText->SetText(FText::FromString(DifficultyDescriptionsMap[Type]));
      return;
    }
  }
  checkNoEntry();
}
void UGameSettingsWidget::Apply() {
  FString SelectedDifficulty = DifficultyComboBox->GetSelectedOption();
  bool bShowTutorials = ShowTutorialCheckBox->IsChecked();

  FGameSettings NewSettings = {
      .Difficulty = (EGameDifficulty)DifficultyComboBox->GetSelectedIndex(),
      .bShowTutorials = bShowTutorials,
  };
  SettingsManager->SetGameSettings(NewSettings);
  SettingsManager->SaveSettings();
}
void UGameSettingsWidget::Back() { BackFunc(); }

void UGameSettingsWidget::RefreshUI() {
  FGameSettings GameSettings = SettingsManager->GameSettings;

  DifficultyComboBox->ClearOptions();
  for (auto Type : TEnumRange<EGameDifficulty>()) DifficultyComboBox->AddOption(DifficultyNamesMap[Type]);
  DifficultyComboBox->SetSelectedIndex((int32)GameSettings.Difficulty);
  DifficultyDescriptionText->SetText(FText::FromString(DifficultyDescriptionsMap[GameSettings.Difficulty]));

  ShowTutorialCheckBox->SetIsChecked(GameSettings.bShowTutorials);
}

void UGameSettingsWidget::InitUI(FInUIInputActions _InUIInputActions,
                                 ASettingsManager* _SettingsManager,
                                 std::function<void()> _BackFunc) {
  check(_SettingsManager && _BackFunc);

  SettingsManager = _SettingsManager;
  BackFunc = _BackFunc;
}

void UGameSettingsWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { Apply(); };
  UIActionable.RetractUI = [this]() { Back(); };
}