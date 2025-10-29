#include "NewGameSetupWidget.h"
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
#include "store_playground/MainMenuControl/MainMenuGameMode.h"

void UNewGameSetupWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  DifficultyComboBox->OnSelectionChanged.AddDynamic(this, &UNewGameSetupWidget::ChangeDifficulty);
  StartButton->OnClicked.AddDynamic(this, &UNewGameSetupWidget::Start);
  BackButton->OnClicked.AddDynamic(this, &UNewGameSetupWidget::Back);

  SetupUIActionable();

  for (auto Type : TEnumRange<EGameDifficulty>())
    check(DifficultyNamesMap.Contains(Type) && DifficultyDescriptionsMap.Contains(Type));
}

void UNewGameSetupWidget::ChangeDifficulty(FString SelectedItem, ESelectInfo::Type SelectionType) {
  if (SelectionType == ESelectInfo::Type::Direct || SelectionType == ESelectInfo::Type::OnKeyPress) return;

  for (auto Type : TEnumRange<EGameDifficulty>()) {
    if (DifficultyNamesMap[Type] == SelectedItem) {
      DifficultyDescriptionText->SetText(FText::FromString(DifficultyDescriptionsMap[Type]));
      return;
    }
  }
  checkNoEntry();
}
void UNewGameSetupWidget::Start() {
  UGameplayStatics::PlaySound2D(this, StartSound, 1.0f);

  FString SelectedDifficulty = DifficultyComboBox->GetSelectedOption();
  bool bShowTutorials = ShowTutorialCheckBox->IsChecked();

  FGameSettings NewSettings = {
      .Difficulty = (EGameDifficulty)DifficultyComboBox->GetSelectedIndex(),
      .bShowTutorials = bShowTutorials,
  };
  SettingsManager->SetGameSettings(NewSettings);
  SettingsManager->SaveSettings();

  AMainMenuGameMode* MainMenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
  check(MainMenuGameMode);
  MainMenuGameMode->StartNewGame();
}
void UNewGameSetupWidget::Back() {
  UGameplayStatics::PlaySound2D(this, BackSound, 1.0f);

  BackFunc();
}

void UNewGameSetupWidget::RefreshUI() {
  FGameSettings GameSettings = SettingsManager->GameSettings;

  DifficultyComboBox->ClearOptions();
  for (auto Type : TEnumRange<EGameDifficulty>()) DifficultyComboBox->AddOption(DifficultyNamesMap[Type]);
  DifficultyComboBox->SetSelectedIndex((int32)GameSettings.Difficulty);
  DifficultyDescriptionText->SetText(FText::FromString(DifficultyDescriptionsMap[GameSettings.Difficulty]));

  ShowTutorialCheckBox->SetIsChecked(GameSettings.bShowTutorials);
}

void UNewGameSetupWidget::InitUI(FInUIInputActions _InUIInputActions,
                                 ASettingsManager* _SettingsManager,
                                 std::function<void()> _BackFunc) {
  check(_SettingsManager && _BackFunc);

  SettingsManager = _SettingsManager;
  BackFunc = _BackFunc;
}

void UNewGameSetupWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { Start(); };
  UIActionable.DirectionalInput = [this](FVector2D Direction) {};
  UIActionable.SideButton4 = [this]() {};
  UIActionable.RetractUI = [this]() { Back(); };
}