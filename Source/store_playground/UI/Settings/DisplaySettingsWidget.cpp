#include "DisplaySettingsWidget.h"
#include "store_playground/Framework/SettingsManager.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"

void UDisplaySettingsWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ApplyButton->OnClicked.AddDynamic(this, &UDisplaySettingsWidget::Apply);
  BackButton->OnClicked.AddDynamic(this, &UDisplaySettingsWidget::Back);

  FrameRateLimitSlider->OnValueChanged.AddDynamic(this, &UDisplaySettingsWidget::OnFrameRateLimitChanged);

  SetupUIActionable();
}

void UDisplaySettingsWidget::Apply() {
  UGameUserSettings* GameSettings = SettingsManager->UnrealSettings;

  if (ResolutionComboBox->GetSelectedIndex() >= 0) {
    FString SelectedResolution = ResolutionComboBox->GetSelectedOption();
    FString Width, Height;
    if (SelectedResolution.Split(TEXT("x"), &Width, &Height)) {
      FIntPoint NewResolution(FCString::Atoi(*Width), FCString::Atoi(*Height));
      GameSettings->SetScreenResolution(NewResolution);
    }
  }
  if (WindowModeComboBox->GetSelectedIndex() >= 0) {
    int32 SelectedIndex = WindowModeComboBox->GetSelectedIndex();
    EWindowMode::Type WindowMode = static_cast<EWindowMode::Type>(SelectedIndex);
    GameSettings->SetFullscreenMode(WindowMode);
  }

  GameSettings->SetVSyncEnabled(VSyncCheckBox->IsChecked());
  GameSettings->SetFrameRateLimit(FrameRateLimitSlider->GetValue());

  if (HDRCheckBox->IsChecked() && GameSettings->SupportsHDRDisplayOutput() &&
      WindowModeComboBox->GetSelectedIndex() == 0)
    GameSettings->EnableHDRDisplayOutput(true);
  else HDRCheckBox->SetIsChecked(false);

  SettingsManager->SaveSettings();
}

void UDisplaySettingsWidget::Back() { BackFunc(); }

void UDisplaySettingsWidget::OnFrameRateLimitChanged(float Value) {
  if (Value >= 999.0f) FrameRateLimitText->SetText(FText::FromString(TEXT("Unlimited")));
  else if (Value < 1.0f) FrameRateLimitText->SetText(FText::FromString(TEXT("Unlimited")));
  else FrameRateLimitText->SetText(FText::FromString(FString::Printf(TEXT("%.0f FPS"), Value)));
}

void UDisplaySettingsWidget::RefreshUI() {
  UGameUserSettings* GameSettings = SettingsManager->UnrealSettings;

  TArray<FIntPoint> CommonResolutions = {
      FIntPoint(3840, 2160),  // 4K
      FIntPoint(2560, 1440),  // 1440p
      FIntPoint(1920, 1080),  // 1080p
      FIntPoint(1680, 1050), FIntPoint(1600, 900), FIntPoint(1440, 900),
      FIntPoint(1366, 768),  FIntPoint(1280, 720)  // 720p
  };
  FIntPoint CurrentResolution = GameSettings->GetScreenResolution();
  int32 CurrentResolutionIndex = 0;

  ResolutionComboBox->ClearOptions();
  for (int32 i = 0; i < CommonResolutions.Num(); i++) {
    FString ResolutionString = FString::Printf(TEXT("%dx%d"), CommonResolutions[i].X, CommonResolutions[i].Y);
    ResolutionComboBox->AddOption(ResolutionString);

    if (CommonResolutions[i] == CurrentResolution) CurrentResolutionIndex = i;
  }
  ResolutionComboBox->SetSelectedIndex(CurrentResolutionIndex);

  EWindowMode::Type CurrentWindowMode = GameSettings->GetFullscreenMode();
  WindowModeComboBox->SetSelectedIndex(static_cast<int32>(CurrentWindowMode));

  VSyncCheckBox->SetIsChecked(GameSettings->IsVSyncEnabled());

  float CurrentFrameRateLimit = GameSettings->GetFrameRateLimit();
  FrameRateLimitSlider->SetValue(CurrentFrameRateLimit);
  OnFrameRateLimitChanged(CurrentFrameRateLimit);

  if (SettingsManager->UnrealSettings->SupportsHDRDisplayOutput())
    HDRCheckBox->SetIsChecked(GameSettings->IsHDREnabled());
  else HDRCheckBox->SetIsEnabled(false);
}

void UDisplaySettingsWidget::InitUI(FInUIInputActions _InUIInputActions,
                                    ASettingsManager* _SettingsManager,
                                    std::function<void()> _BackFunc) {
  check(_SettingsManager && _BackFunc);

  SettingsManager = _SettingsManager;
  BackFunc = _BackFunc;

  WindowModeComboBox->ClearOptions();
  WindowModeComboBox->AddOption(TEXT("Fullscreen"));
  WindowModeComboBox->AddOption(TEXT("Windowed Fullscreen"));
  WindowModeComboBox->AddOption(TEXT("Windowed"));
}

void UDisplaySettingsWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { Apply(); };
  UIActionable.RetractUI = [this]() { Back(); };
}