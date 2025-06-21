#include "SaveLoadConfirmWidget.h"
#include "store_playground/UI/SaveSlots/PopulatedSaveSlotWidget.h"
#include "store_playground/UI/SaveSlots/EmptySaveSlotWidget.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/SaveManager/SaveManager.h"
#include "store_playground/SaveManager/SaveStructs.h"
#include "store_playground/SaveManager/SaveSlotListSaveGame.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "store_playground/Framework/StorePGGameMode.h"
#include "Kismet/GameplayStatics.h"

void USaveLoadConfirmWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  ConfirmButton->ControlButton->OnClicked.AddDynamic(this, &USaveLoadConfirmWidget::Confirm);
  CancelButton->ControlButton->OnClicked.AddDynamic(this, &USaveLoadConfirmWidget::Cancel);
}

void USaveLoadConfirmWidget::InitUI(FText _Text,
                                    std::function<void()> _ConfirmFunc,
                                    std::function<void()> _CancelFunc) {
  check(_ConfirmFunc && _CancelFunc);

  Text->SetText(_Text);
  ConfirmButton->ActionText->SetText(NSLOCTEXT("SaveLoadConfirmWidget", "Confirm", "Confirm"));
  CancelButton->ActionText->SetText(NSLOCTEXT("SaveLoadConfirmWidget", "Cancel", "Cancel"));

  ConfirmFunc = _ConfirmFunc;
  CancelFunc = _CancelFunc;
}

void USaveLoadConfirmWidget::Confirm() { ConfirmFunc(); }
void USaveLoadConfirmWidget::Cancel() { CancelFunc(); }