#include "TutorialViewWidget.h"
#include "TimerManager.h"
#include "store_playground/Ability/AbilityManager.h"
#include "store_playground/UI/Tutorial/TutorialStepWidget.h"
#include "store_playground/UI/Components/RightSlideWidget.h"
#include "store_playground/UI/Components/ControlMenuButtonWidget.h"
#include "store_playground/UI/Components/ControlsHelpersWidget.h"
#include "store_playground/UI/Negotiation/PriceSliderWidget.h"
#include "store_playground/UI/Inventory/CompactItemDetailsWidget.h"
#include "store_playground/UI/Negotiation/NegotiationSkillsWidget.h"
#include "store_playground/UI/Negotiation/NegotiationShowItemWidget.h"
#include "store_playground/UI/Negotiation/PriceNegotiationWidget.h"
#include "store_playground/UI/Dialogue/DialogueWidget.h"
#include "store_playground/Negotiation/NegotiationSystem.h"
#include "store_playground/AI/CustomerAIComponent.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Market/MarketEconomy.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/ScrollBox.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "CommonVideoPlayer.h"
#include "Logging/LogVerbosity.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/AssertionMacros.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Kismet/GameplayStatics.h"

void UTutorialViewWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  NextButton->OnClicked.AddDynamic(this, &UTutorialViewWidget::Next);
  PrevButton->OnClicked.AddDynamic(this, &UTutorialViewWidget::Prev);
  CloseButton->OnClicked.AddDynamic(this, &UTutorialViewWidget::Close);

  SetupUIActionable();
  SetupUIBehaviour();
}

void UTutorialViewWidget::Next() {
  if (CurrentStepIndex + 1 >= TutorialSteps.Num()) return;

  CurrentStepIndex++;
  RefreshUI();

  UGameplayStatics::PlaySound2D(this, NextSound, 1.0f);
}
void UTutorialViewWidget::Prev() {
  if (CurrentStepIndex - 1 < 0) return;

  CurrentStepIndex--;
  RefreshUI();

  UGameplayStatics::PlaySound2D(this, NextSound, 1.0f);
}

void UTutorialViewWidget::Close() {
  GetWorld()->GetTimerManager().ClearTimer(VideoTimerHandle);

  CloseWidgetFunc();
}

void UTutorialViewWidget::RefreshUI() {
  FUITutorialStep CurrentStep = TutorialSteps[CurrentStepIndex];

  TutorialStepWidget->TitleText->SetText(CurrentStep.Title);
  TutorialStepWidget->BodyText->SetText(CurrentStep.Body);
  if (CurrentStep.Image) {
    TutorialStepWidget->TutorialImage->SetBrushFromTexture(CurrentStep.Image);
    TutorialStepWidget->TutorialImage->SetVisibility(ESlateVisibility::Visible);
    TutorialStepWidget->TutorialVideoPlayer->SetVisibility(ESlateVisibility::Collapsed);
  } else if (CurrentStep.Video) {
    TutorialStepWidget->TutorialVideoPlayer->SetVideo(CurrentStep.Video);
    TutorialStepWidget->TutorialVideoPlayer->SetIsMuted(true);
    TutorialStepWidget->TutorialVideoPlayer->SetLooping(true);

    // ! Only way to wait for the video to be loaded.
    GetWorld()->GetTimerManager().ClearTimer(VideoTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(VideoTimerHandle, FTimerDelegate::CreateLambda([this] {
                                             UE_LOG(LogTemp, Warning, TEXT("Playing video"));
                                             TutorialStepWidget->TutorialVideoPlayer->PlayFromStart();
                                           }),
                                           0.2, false);

    TutorialStepWidget->TutorialImage->SetVisibility(ESlateVisibility::Collapsed);
    TutorialStepWidget->TutorialVideoPlayer->SetVisibility(ESlateVisibility::Visible);
  } else {
    TutorialStepWidget->TutorialImage->SetVisibility(ESlateVisibility::Collapsed);
    TutorialStepWidget->TutorialVideoPlayer->SetVisibility(ESlateVisibility::Collapsed);
  }
  StepsScrollBox->ScrollToStart();

  NextButton->SetIsEnabled(TutorialSteps.Num() > CurrentStepIndex + 1);
  PrevButton->SetIsEnabled(CurrentStepIndex - 1 >= 0);
}

void UTutorialViewWidget::InitUI(FInUIInputActions _InUIInputActions,
                                 TArray<FUITutorialStep> _TutorialSteps,
                                 std::function<void()> _CloseWidgetFunc) {
  check(_TutorialSteps.Num() > 0 && _CloseWidgetFunc);

  TutorialSteps = _TutorialSteps;
  CloseWidgetFunc = _CloseWidgetFunc;

  CurrentStepIndex = 0;
}

void UTutorialViewWidget::SetupUIActionable() {
  UIActionable.AdvanceUI = [this]() {
    if (NextButton->GetIsEnabled()) Next();
    else Close();
  };
  UIActionable.RetractUI = [this]() {
    if (PrevButton->GetIsEnabled()) Prev();
    else Close();
  };
  UIActionable.QuitUI = [this]() { Close(); };
}
void UTutorialViewWidget::SetupUIBehaviour() {
  UIBehaviour.ShowAnim = ShowAnim;
  UIBehaviour.HideAnim = HideAnim;
}