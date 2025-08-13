#include "SoundSettingsWidget.h"
#include "store_playground/Framework/SettingsManager.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Slider.h"

void USoundSettingsWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ApplyButton->OnClicked.AddDynamic(this, &USoundSettingsWidget::Apply);
  BackButton->OnClicked.AddDynamic(this, &USoundSettingsWidget::Back);

  SetupUIActionable();
}

void USoundSettingsWidget::Apply() {
  SettingsManager->SetMasterVolume(MasterVolumeSlider->GetValue());
  SettingsManager->SetMusicVolume(MusicVolumeSlider->GetValue());
  SettingsManager->SetSFXVolume(SFXVolumeSlider->GetValue());

  SettingsManager->SaveSettings();
}

void USoundSettingsWidget::Back() { BackFunc(); }

void USoundSettingsWidget::RefreshUI() {
  MasterVolumeSlider->SetValue(SettingsManager->SoundSettings.MasterVolume);
  MusicVolumeSlider->SetValue(SettingsManager->SoundSettings.MusicVolume);
  SFXVolumeSlider->SetValue(SettingsManager->SoundSettings.SFXVolume);

  MasterVolumeSlider->SetFocus();
}

void USoundSettingsWidget::InitUI(FInUIInputActions _InUIInputActions,
                                  ASettingsManager* _SettingsManager,
                                  std::function<void()> _BackFunc) {
  check(_SettingsManager && _BackFunc);

  SettingsManager = _SettingsManager;
  BackFunc = _BackFunc;
}

void USoundSettingsWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { Apply(); };
  UIActionable.RetractUI = [this]() { Back(); };
}