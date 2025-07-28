#include "GraphicsSettingsWidget.h"
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

inline void PopulateQualityComboBox(UComboBoxString* ComboBox, int32 CurrentValue) {
  ComboBox->ClearOptions();
  ComboBox->AddOption(TEXT("Low"));
  ComboBox->AddOption(TEXT("Medium"));
  ComboBox->AddOption(TEXT("High"));
  ComboBox->AddOption(TEXT("Epic"));
  ComboBox->AddOption(TEXT("Cinematic"));

  // testing the

  int32 ClampedValue = FMath::Clamp(CurrentValue, 0, 4);
  ComboBox->SetSelectedIndex(ClampedValue);
}

void UGraphicsSettingsWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ApplyButton->OnClicked.AddDynamic(this, &UGraphicsSettingsWidget::Apply);
  BackButton->OnClicked.AddDynamic(this, &UGraphicsSettingsWidget::Back);

  OverallQualityComboBox->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnOverallQualityChanged);
  ResolutionScaleSlider->OnValueChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnResolutionScaleChanged);
  GlobalIlluminationMethodComboBox->OnSelectionChanged.AddDynamic(
      this, &UGraphicsSettingsWidget::OnGlobalIlluminationMethodChanged);
  ReflectionMethodComboBox->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidget::OnReflectionMethodChanged);

  ResolutionScaleSlider->SetMinValue(0.25f);
  ResolutionScaleSlider->SetMaxValue(1.5f);
}

void UGraphicsSettingsWidget::Apply() {
  UGameUserSettings* GameSettings = SettingsManager->UnrealSettings;

  if (OverallQualityComboBox->GetSelectedIndex() != 5)
    GameSettings->SetOverallScalabilityLevel(OverallQualityComboBox->GetSelectedIndex());

  GameSettings->SetViewDistanceQuality(ViewDistanceComboBox->GetSelectedIndex());
  GameSettings->SetShadowQuality(ShadowQualityComboBox->GetSelectedIndex());
  GameSettings->SetGlobalIlluminationQuality(GlobalIlluminationComboBox->GetSelectedIndex());
  GameSettings->SetReflectionQuality(ReflectionQualityComboBox->GetSelectedIndex());
  GameSettings->SetAntiAliasingQuality(AntiAliasingComboBox->GetSelectedIndex());
  GameSettings->SetTextureQuality(TextureQualityComboBox->GetSelectedIndex());
  GameSettings->SetVisualEffectQuality(VisualEffectsComboBox->GetSelectedIndex());
  GameSettings->SetPostProcessingQuality(PostProcessingComboBox->GetSelectedIndex());
  GameSettings->SetFoliageQuality(FoliageQualityComboBox->GetSelectedIndex());
  GameSettings->SetShadingQuality(ShadingQualityComboBox->GetSelectedIndex());

  int32 AAIndex = AntiAliasingMethodComboBox->GetSelectedIndex();
  switch (AAIndex) {
    case 0: SettingsManager->SetAntiAliasingMethod(0); break;
    case 1: SettingsManager->SetAntiAliasingMethod(1); break;
    case 2: SettingsManager->SetAntiAliasingMethod(4); break;
    default: checkNoEntry();
  }

  GameSettings->SetResolutionScaleNormalized(ResolutionScaleSlider->GetValue());
  SettingsManager->SetGlobalIlluminationMethod(GlobalIlluminationMethodComboBox->GetSelectedIndex());
  SettingsManager->SetReflectionMethod(ReflectionMethodComboBox->GetSelectedIndex());
  SettingsManager->SetMotionBlurEnabled(MotionBlurCheckBox->IsChecked());
  SettingsManager->SetBloomEnabled(BloomCheckBox->IsChecked());

  SettingsManager->SaveSettings();
}

void UGraphicsSettingsWidget::Back() { BackFunc(); }

void UGraphicsSettingsWidget::OnOverallQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType) {
  if (SelectedItem.IsEmpty() || SelectionType == ESelectInfo::Direct) return;
  UE_LOG(LogTemp, Log, TEXT("Overall Quality changed to: %s"), *SelectedItem);

  int32 QualityLevel = OverallQualityComboBox->GetSelectedIndex();
  if (QualityLevel == 5) return;  // Custom quality, do not change anything

  // Set advanced settings based on quality level
  if (QualityLevel >= 3)                                    // Epic and Cinematic use Lumen
    GlobalIlluminationMethodComboBox->SetSelectedIndex(1);  // Lumen
  else if (QualityLevel >= 2)                               // High use Screen Space
    GlobalIlluminationMethodComboBox->SetSelectedIndex(2);  // Screen Space
  else                                                      // Low and Medium uses None
    GlobalIlluminationMethodComboBox->SetSelectedIndex(0);  // None
  if (QualityLevel >= 3)                                    // Epic and Cinematic use Lumen
    ReflectionMethodComboBox->SetSelectedIndex(1);          // Lumen
  else if (QualityLevel >= 2)                               // High uses Screen Space
    ReflectionMethodComboBox->SetSelectedIndex(2);          // Screen Space
  else                                                      // Low and Medium use None
    ReflectionMethodComboBox->SetSelectedIndex(0);          // None
  MotionBlurCheckBox->SetIsChecked(false);
  BloomCheckBox->SetIsChecked(QualityLevel >= 1);  // Bloom for Medium and above

  PopulateQualityComboBox(ViewDistanceComboBox, QualityLevel);
  PopulateQualityComboBox(ShadowQualityComboBox, QualityLevel);
  PopulateQualityComboBox(GlobalIlluminationComboBox, QualityLevel);
  PopulateQualityComboBox(ReflectionQualityComboBox, QualityLevel);
  PopulateQualityComboBox(AntiAliasingComboBox, QualityLevel);
  PopulateQualityComboBox(TextureQualityComboBox, QualityLevel);
  PopulateQualityComboBox(VisualEffectsComboBox, QualityLevel);
  PopulateQualityComboBox(PostProcessingComboBox, QualityLevel);
  PopulateQualityComboBox(FoliageQualityComboBox, QualityLevel);
  PopulateQualityComboBox(ShadingQualityComboBox, QualityLevel);
}

void UGraphicsSettingsWidget::OnResolutionScaleChanged(float Value) {
  int32 Percentage = FMath::RoundToInt(Value * 100.0f);
  ResolutionScaleText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), Percentage)));
}

void UGraphicsSettingsWidget::OnGlobalIlluminationMethodChanged(FString SelectedItem, ESelectInfo::Type SelectionType) {
  if (SelectedItem.IsEmpty() || SelectionType == ESelectInfo::Direct) return;

  int32 SelectedIndex = GlobalIlluminationMethodComboBox->GetSelectedIndex();
  SettingsManager->SetGlobalIlluminationMethod(SelectedIndex);
}

void UGraphicsSettingsWidget::OnReflectionMethodChanged(FString SelectedItem, ESelectInfo::Type SelectionType) {
  if (SelectedItem.IsEmpty() || SelectionType == ESelectInfo::Direct) return;

  int32 SelectedIndex = ReflectionMethodComboBox->GetSelectedIndex();
  SettingsManager->SetReflectionMethod(SelectedIndex);
}

void UGraphicsSettingsWidget::RefreshUI() {
  UGameUserSettings* GameSettings = SettingsManager->UnrealSettings;

  int32 CurrentOverallQuality = GameSettings->GetOverallScalabilityLevel();
  UE_LOG(LogTemp, Log, TEXT("Current Overall Quality: %d"), CurrentOverallQuality);
  OverallQualityComboBox->SetSelectedIndex(CurrentOverallQuality == -1 ? 5 : CurrentOverallQuality);

  PopulateQualityComboBox(ViewDistanceComboBox, GameSettings->GetViewDistanceQuality());
  PopulateQualityComboBox(ShadowQualityComboBox, GameSettings->GetShadowQuality());
  PopulateQualityComboBox(GlobalIlluminationComboBox, GameSettings->GetGlobalIlluminationQuality());
  PopulateQualityComboBox(ReflectionQualityComboBox, GameSettings->GetReflectionQuality());
  PopulateQualityComboBox(TextureQualityComboBox, GameSettings->GetTextureQuality());
  PopulateQualityComboBox(VisualEffectsComboBox, GameSettings->GetVisualEffectQuality());
  PopulateQualityComboBox(PostProcessingComboBox, GameSettings->GetPostProcessingQuality());
  PopulateQualityComboBox(FoliageQualityComboBox, GameSettings->GetFoliageQuality());
  PopulateQualityComboBox(ShadingQualityComboBox, GameSettings->GetShadingQuality());
  PopulateQualityComboBox(AntiAliasingComboBox, GameSettings->GetAntiAliasingQuality());

  static IConsoleVariable* GetAA = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
  int32 AAIndex = GetAA->GetInt();
  switch (AAIndex) {
    case 0:  // None
      AntiAliasingMethodComboBox->SetSelectedIndex(0);
      break;
    case 1:  // FXAA
      AntiAliasingMethodComboBox->SetSelectedIndex(1);
      break;
    case 4:  // TSR (Temporal Super Resolution)
      AntiAliasingMethodComboBox->SetSelectedIndex(2);
      break;
    default:  // Other methods not offered in the UI.
      AntiAliasingMethodComboBox->SetSelectedIndex(1);
  }

  // Populate advanced settings
  float CurrentScale = GameSettings->GetResolutionScaleNormalized();
  ResolutionScaleSlider->SetValue(CurrentScale);
  OnResolutionScaleChanged(CurrentScale);

  static IConsoleVariable* GI = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DynamicGlobalIlluminationMethod"));
  if (GI && GI->GetInt() == 1) GlobalIlluminationMethodComboBox->SetSelectedIndex(1);       // Lumen
  else if (GI && GI->GetInt() == 2) GlobalIlluminationMethodComboBox->SetSelectedIndex(2);  // Screen Space
  else GlobalIlluminationMethodComboBox->SetSelectedIndex(0);                               // None

  static IConsoleVariable* Refl = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ReflectionMethod"));
  if (Refl && Refl->GetInt() == 1) ReflectionMethodComboBox->SetSelectedIndex(1);       // Lumen
  else if (Refl && Refl->GetInt() == 2) ReflectionMethodComboBox->SetSelectedIndex(2);  // Screen Space
  else ReflectionMethodComboBox->SetSelectedIndex(0);                                   // None

  static IConsoleVariable* MotionBlur = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlur.Amount"));
  MotionBlurCheckBox->SetIsChecked(MotionBlur && MotionBlur->GetFloat() > 0.0f);

  static IConsoleVariable* Bloom = IConsoleManager::Get().FindConsoleVariable(TEXT("r.BloomQuality"));
  BloomCheckBox->SetIsChecked(Bloom && Bloom->GetInt() > 0);
}

void UGraphicsSettingsWidget::InitUI(FInUIInputActions _InUIInputActions,
                                     ASettingsManager* _SettingsManager,
                                     std::function<void()> _BackFunc) {
  check(_SettingsManager && _BackFunc);

  SettingsManager = _SettingsManager;
  BackFunc = _BackFunc;

  OverallQualityComboBox->ClearOptions();
  OverallQualityComboBox->AddOption(TEXT("Low"));
  OverallQualityComboBox->AddOption(TEXT("Medium"));
  OverallQualityComboBox->AddOption(TEXT("High"));
  OverallQualityComboBox->AddOption(TEXT("Epic"));
  OverallQualityComboBox->AddOption(TEXT("Cinematic"));
  OverallQualityComboBox->AddOption(TEXT("Custom"));

  AntiAliasingMethodComboBox->ClearOptions();
  AntiAliasingMethodComboBox->AddOption(TEXT("None"));
  AntiAliasingMethodComboBox->AddOption(TEXT("FXAA"));
  AntiAliasingMethodComboBox->AddOption(TEXT("TSR"));

  GlobalIlluminationMethodComboBox->ClearOptions();
  GlobalIlluminationMethodComboBox->AddOption(TEXT("None"));
  GlobalIlluminationMethodComboBox->AddOption(TEXT("Lumen"));
  GlobalIlluminationMethodComboBox->AddOption(TEXT("Screen Space"));

  ReflectionMethodComboBox->ClearOptions();
  ReflectionMethodComboBox->AddOption(TEXT("None"));
  ReflectionMethodComboBox->AddOption(TEXT("Lumen"));
  ReflectionMethodComboBox->AddOption(TEXT("Screen Space"));
}