#include "InGameHudWidget.h"
#include "Blueprint/UserWidget.h"
#include "Internationalization/Text.h"
#include "Logging/LogVerbosity.h"
#include "TimerManager.h"
#include "store_playground/UI/Components/ControlsHelpersWidget.h"
#include "store_playground/UI/Components/LeftSlideWidget.h"
#include "store_playground/UI/Components/RightSlideWidget.h"
#include "store_playground/UI/Components/RightSlideSecondaryWidget.h"
#include "store_playground/UI/Components/RightSlideInvertedWidget.h"
#include "store_playground/UI/InGameHud/InGameHudWidgets/NewsHudSlideWidget.h"
#include "store_playground/Store/Store.h"
#include "store_playground/Upgrade/UpgradeManager.h"
#include "store_playground/DayManager/DayManager.h"
#include "store_playground/Framework/StorePhaseManager.h"
#include "store_playground/Level/LevelManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UInGameHudWidget::NativeOnInitialized() {
  Super::NativeOnInitialized();

  FWidgetAnimationDynamicEvent UIAnimEvent;
  UIAnimEvent.BindDynamic(this, &UInGameHudWidget::HideAnimComplete);
  BindToAnimationFinished(HideAnim, UIAnimEvent);
}

void UInGameHudWidget::NotifyUpgradePointsGained() {
  if (!bShowingHud) {
    bNeedUpgradePointsNotify = true;
    return;
  }

  NotificationsSlideWidget->SlideText->SetText(FText::FromString("Upgrade Points Gained!"));
  NotificationsSlideWidget->RightSlideText->SetText(FText::FromString(""));
  NotificationsSlideWidget->SetVisibility(ESlateVisibility::Visible);
  UGameplayStatics::PlaySound2D(this, NotifySound, 1.0f);

  GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &UInGameHudWidget::HideUpgradePointsNotify, 10.0f,
                                         false);
}
void UInGameHudWidget::HideUpgradePointsNotify() {
  NotificationsSlideWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UInGameHudWidget::RefreshUI() {
  check(UpgradeManager && DayManager && StorePhaseManager && Store && LevelManager);
  UE_LOG(LogTemp, Log, TEXT("Refreshing InGame HUD UI."));

  UpgradePointsSlideWidget->SlideText->SetText(
      FText::FromString(FString::Printf(TEXT("Upgrade Points: %d"), UpgradeManager->AvailableUpgradePoints)));
  UpgradePointsSlideWidget->RightSlideText->SetText(FText::FromString(""));

  FText CurrentPhaseText = GetStorePhaseText(StorePhaseManager->StorePhaseState);
  int32 CurrentDay = DayManager->CurrentDay;
  DaySlideWidget->SlideText->SetText(
      FText::FromString(FString::Printf(TEXT("%s - Day: %d"), *CurrentPhaseText.ToString(), CurrentDay)));
  DaySlideWidget->RightSlideText->SetText(FText::FromString(DayManager->bIsWeekend ? "Weekend" : "Normal Day"));

  OwnedSlideWidget->SlideText->SetText(
      FText::FromString(FString::Printf(TEXT("Owed: %.0f¬"), DayManager->NextDebtAmount)));
  OwnedSlideWidget->RightSlideText->SetText(
      FText::FromString(FString::Printf(TEXT("On Day: %d"), DayManager->NextDayToPayDebt)));

  MoneySlideWidget->SlideText->SetText(FText::FromString(FString::Printf(TEXT("Money: %.0f¬"), Store->Money)));
  MoneySlideWidget->RightSlideText->SetText(FText::FromString(""));

  FText CurrentLevelText = GetLevelText(LevelManager->LoadedLevel);
  LocationSlideWidget->SlideText->SetText(CurrentLevelText);

  if (LevelManager->LoadedLevel == ELevel::Store &&
      (StorePhaseManager->StorePhaseState == EStorePhaseState::Morning ||
       StorePhaseManager->StorePhaseState == EStorePhaseState::MorningBuildMode)) {
    ControlsHelpersWidget->SetComponentUI({
        {FText::FromString("News"), InGameInputActions.OpenNewspaperAction},
        {FText::FromString("Store"), InGameInputActions.OpenStoreViewAction},
        {FText::FromString("Items"), InGameInputActions.OpenInventoryViewAction},
        {FText::FromString("Build"), InGameInputActions.BuildModeAction},
    });
  } else {
    ControlsHelpersWidget->SetComponentUI({
        {FText::FromString("News"), InGameInputActions.OpenNewspaperAction},
        {FText::FromString("Store"), InGameInputActions.OpenStoreViewAction},
        {FText::FromString("Items"), InGameInputActions.OpenInventoryViewAction},
    });
  }
}

void UInGameHudWidget::InitUI(FInGameInputActions _InGameInputActions) {
  InGameInputActions = _InGameInputActions;

  NotificationsSlideWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UInGameHudWidget::Show() {
  bShowingHud = true;

  SetVisibility(ESlateVisibility::Visible);
  PlayAnimation(ShowAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);

  if (bNeedUpgradePointsNotify) {
    NotifyUpgradePointsGained();
    bNeedUpgradePointsNotify = false;
  }
}
void UInGameHudWidget::Hide() {
  bShowingHud = false;

  PlayAnimation(HideAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f);
}
void UInGameHudWidget::HideAnimComplete() {
  if (!bShowingHud) SetVisibility(ESlateVisibility::Collapsed);
  else check(GetVisibility() == ESlateVisibility::Visible);
}
