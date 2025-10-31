#include "ControlsSettingsWidget.h"
#include "CoreGlobals.h"
#include "GameplayTagContainer.h"
#include "InputCoreTypes.h"
#include "Internationalization/Text.h"
#include "Logging/LogVerbosity.h"
#include "Misc/AssertionMacros.h"
#include "store_playground/Framework/SettingsManager.h"
#include "store_playground/UI/Settings/ControlsRebindWidget.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Slider.h"

inline bool HasDuplicateKeyInCategory(const FKey& Key,
                                      const FText& Category,
                                      const FName& ExcludeMappingName,
                                      UEnhancedPlayerMappableKeyProfile* Profile) {
  FString CategoryString = Category.ToString();
  if (CategoryString == GetInputTypeNameText(EInputTypeNames::InUINonExclusive).ToString()) return false;

  for (const TPair<FName, FKeyMappingRow>& RowPair : Profile->GetPlayerMappingRows()) {
    for (const FPlayerKeyMapping& Mapping : RowPair.Value.Mappings) {
      if (ExcludeMappingName != NAME_None && Mapping.GetMappingName() == ExcludeMappingName) continue;

      if (Mapping.GetCurrentKey() == Key && Mapping.GetDisplayCategory().ToString() == Category.ToString()) return true;
    }
  }

  return false;
}

void UControlsSettingsWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ApplyButton->OnClicked.AddDynamic(this, &UControlsSettingsWidget::Apply);
  ResetButton->OnClicked.AddDynamic(this, &UControlsSettingsWidget::Reset);
  BackButton->OnClicked.AddDynamic(this, &UControlsSettingsWidget::Back);

  SetupUIActionable();
}

void UControlsSettingsWidget::Apply() {
  UEnhancedPlayerMappableKeyProfile* CurrentProfile = SettingsManager->EInputUserSettings->GetCurrentKeyProfile();
  for (const TPair<FName, FKeyMappingRow>& RowPair : CurrentProfile->GetPlayerMappingRows()) {
    for (const FPlayerKeyMapping& Mapping : RowPair.Value.Mappings) {
      FKey CurrentKey = Mapping.GetCurrentKey();
      if (HasDuplicateKeyInCategory(CurrentKey, Mapping.GetDisplayCategory(), Mapping.GetMappingName(),
                                    SettingsManager->EInputUserSettings->GetCurrentKeyProfile())) {
        ErrorText->SetText(FText::Format(FText::FromString("Key '{0}' is already bound to another action"),
                                         FText::FromString(CurrentKey.GetDisplayName().ToString())));
        ErrorText->SetVisibility(ESlateVisibility::Visible);

        return;
      }
    }
  }

  SettingsManager->SaveSettings();
  ErrorText->SetVisibility(ESlateVisibility::Hidden);
}
void UControlsSettingsWidget::Reset() {
  ErrorText->SetVisibility(ESlateVisibility::Hidden);

  UEnhancedInputUserSettings* EInputUserSettings = SettingsManager->EInputUserSettings;
  for (const TPair<FGameplayTag, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& ProfilePair :
       EInputUserSettings->GetAllSavedKeyProfiles()) {
    const TObjectPtr<UEnhancedPlayerMappableKeyProfile>& Profile = ProfilePair.Value;
    FGameplayTagContainer FailureReason;
    EInputUserSettings->ResetKeyProfileToDefault(Profile->GetProfileIdentifer(), FailureReason);
  }

  RefreshUI();
}
void UControlsSettingsWidget::Back() { BackFunc(); }

void UControlsSettingsWidget::RebindKey(FPlayerKeyMapping KeyMapping, FInputChord InputChord) {
  ErrorText->SetVisibility(ESlateVisibility::Hidden);
  FText KeyCategory = KeyMapping.GetDisplayCategory();
  if (HasDuplicateKeyInCategory(InputChord.Key, KeyCategory, KeyMapping.GetMappingName(),
                                SettingsManager->EInputUserSettings->GetCurrentKeyProfile())) {
    ErrorText->SetText(FText::Format(FText::FromString("Key '{0}' is already bound to another '{1}' action"),
                                     FText::FromString(InputChord.Key.GetDisplayName().ToString()), KeyCategory));
    ErrorText->SetVisibility(ESlateVisibility::Visible);
  }

  FMapPlayerKeyArgs Args;
  Args.MappingName = KeyMapping.GetMappingName();
  Args.Slot = EPlayerMappableKeySlot::First;
  Args.NewKey = InputChord.Key;

  UEnhancedInputUserSettings* EInputUserSettings = SettingsManager->EInputUserSettings;
  check(EInputUserSettings);
  FGameplayTagContainer FailureReason;
  EInputUserSettings->MapPlayerKey(Args, FailureReason);

  if (!FailureReason.IsEmpty()) {
    ErrorText->SetText(FText::Format(FText::FromString("Failed to rebind key: {0}"), InputChord.Key.GetDisplayName()));
    ErrorText->SetVisibility(ESlateVisibility::Visible);
    return;
  }

  RefreshUI();
}
void UControlsSettingsWidget::ResetKey(FPlayerKeyMapping KeyMapping) {
  ErrorText->SetVisibility(ESlateVisibility::Hidden);

  FMapPlayerKeyArgs Args;
  Args.MappingName = KeyMapping.GetMappingName();
  Args.Slot = EPlayerMappableKeySlot::First;
  Args.NewKey = KeyMapping.GetDefaultKey();

  UEnhancedInputUserSettings* EInputUserSettings = SettingsManager->EInputUserSettings;
  check(EInputUserSettings);
  FGameplayTagContainer FailureReason;
  EInputUserSettings->MapPlayerKey(Args, FailureReason);

  if (!FailureReason.IsEmpty()) {
    ErrorText->SetText(FText::Format(FText::FromString("Failed to rebind key: {0}"), KeyMapping.GetDisplayName()));
    ErrorText->SetVisibility(ESlateVisibility::Visible);
    return;
  }

  RefreshUI();
}

void UControlsSettingsWidget::RefreshUI() {
  InGameControlsPanel->ClearChildren();
  InUIControlsPanel->ClearChildren();
  InCutsceneControlsPanel->ClearChildren();

  UEnhancedPlayerMappableKeyProfile* CurrentProfile = SettingsManager->EInputUserSettings->GetCurrentKeyProfile();

  const TMap<FName, FKeyMappingRow>& PlayerMappingRows = CurrentProfile->GetPlayerMappingRows();
  TArray<FKeyMappingRow> SortedKeys;
  PlayerMappingRows.GenerateValueArray(SortedKeys);
  SortedKeys.Sort([](const FKeyMappingRow& A, const FKeyMappingRow& B) {
    TArray<FPlayerKeyMapping> AMappingsArray = A.Mappings.Array();
    TArray<FPlayerKeyMapping> BMappingsArray = B.Mappings.Array();
    return AMappingsArray.Num() > BMappingsArray.Num()
               ? true
               : (AMappingsArray.Num() < BMappingsArray.Num()
                      ? false
                      : AMappingsArray[0].GetMappingName().ToString() < BMappingsArray[0].GetMappingName().ToString());
  });
  for (const FKeyMappingRow& KeyRow : SortedKeys) {
    for (const FPlayerKeyMapping& Mapping : KeyRow.Mappings) {
      UControlsRebindWidget* RebindWidget = CreateWidget<UControlsRebindWidget>(this, ControlsRebindWidgetClass);
      RebindWidget->InitUI(
          Mapping, [this](FPlayerKeyMapping KeyMapping, FInputChord Chord) { RebindKey(KeyMapping, Chord); },
          [this](FPlayerKeyMapping KeyMapping) { ResetKey(KeyMapping); });

      FString CategoryString = Mapping.GetDisplayCategory().ToString();
      check(!CategoryString.IsEmpty());

      if (CategoryString == GetInputTypeNameText(EInputTypeNames::InGame).ToString())
        InGameControlsPanel->AddChildToVerticalBox(RebindWidget);
      else if (CategoryString == GetInputTypeNameText(EInputTypeNames::InUI).ToString())
        InUIControlsPanel->AddChildToVerticalBox(RebindWidget);
      else if (CategoryString == GetInputTypeNameText(EInputTypeNames::InUINonExclusive).ToString())
        InUIControlsPanel->AddChildToVerticalBox(RebindWidget);
      else if (CategoryString == GetInputTypeNameText(EInputTypeNames::InCutscene).ToString())
        InCutsceneControlsPanel->AddChildToVerticalBox(RebindWidget);
      else UE_LOG(LogTemp, Error, TEXT("Unknown input type: %s"), *CategoryString);
    }
  }

  // Check for duplicate keys.
  for (const TPair<FName, FKeyMappingRow>& RowPair : CurrentProfile->GetPlayerMappingRows()) {
    for (const FPlayerKeyMapping& Mapping : RowPair.Value.Mappings) {
      FKey CurrentKey = Mapping.GetCurrentKey();
      if (HasDuplicateKeyInCategory(CurrentKey, Mapping.GetDisplayCategory(), Mapping.GetMappingName(),
                                    SettingsManager->EInputUserSettings->GetCurrentKeyProfile())) {
        ErrorText->SetText(FText::Format(FText::FromString("Key '{0}' is already bound to another action"),
                                         FText::FromString(CurrentKey.GetDisplayName().ToString())));
        ErrorText->SetVisibility(ESlateVisibility::Visible);

        return;
      }
    }
  }
}

void UControlsSettingsWidget::InitUI(FInUIInputActions _InUIInputActions,
                                     ASettingsManager* _SettingsManager,
                                     std::function<void()> _BackFunc) {
  check(_SettingsManager && _BackFunc);

  SettingsManager = _SettingsManager;
  BackFunc = _BackFunc;

  ErrorText->SetVisibility(ESlateVisibility::Hidden);
}

void UControlsSettingsWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() { Apply(); };
  UIActionable.RetractUI = [this]() { Back(); };
}