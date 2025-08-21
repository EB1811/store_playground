#include "ControlsRebindWidget.h"
#include "GameplayTagContainer.h"
#include "InputCoreTypes.h"
#include "Logging/LogVerbosity.h"
#include "store_playground/Framework/SettingsManager.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/InputKeySelector.h"

void UControlsRebindWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  RebindKeySelector->OnIsSelectingKeyChanged.AddDynamic(this, &UControlsRebindWidget::IsSelecting);
  RebindKeySelector->OnKeySelected.AddDynamic(this, &UControlsRebindWidget::Rebind);
  ResetButton->OnClicked.AddDynamic(this, &UControlsRebindWidget::Reset);
}

void UControlsRebindWidget::IsSelecting() {
  if (RebindKeySelector->GetIsSelectingKey()) DisplayText->SetColorAndOpacity(IsRebindingColor);
  else DisplayText->SetColorAndOpacity(NormalColor);
}
void UControlsRebindWidget::Rebind(FInputChord SelectedKey) { RebindFunc(KeyMapping, SelectedKey); }
void UControlsRebindWidget::Reset() { ResetFunc(KeyMapping); }

void UControlsRebindWidget::InitUI(FPlayerKeyMapping _KeyMapping,
                                   std::function<void(FPlayerKeyMapping, FInputChord)> _RebindFunc,
                                   std::function<void(FPlayerKeyMapping)> _ResetFunc) {
  check(_RebindFunc && _ResetFunc);

  KeyMapping = _KeyMapping;
  RebindFunc = _RebindFunc;
  ResetFunc = _ResetFunc;

  DisplayText->SetText(KeyMapping.GetDisplayName());

  RebindKeySelector->SetSelectedKey(KeyMapping.GetCurrentKey());
  RebindKeySelector->SetKeySelectionText(KeyMapping.GetCurrentKey().GetDisplayName());

  ResetButton->SetIsEnabled(!KeyMapping.GetCurrentKey().IsValid() ||
                            KeyMapping.GetCurrentKey() != KeyMapping.GetDefaultKey());
}