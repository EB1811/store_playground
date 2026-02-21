#include "AnyKeyTransitionWidget.h"
#include "Components/InputKeySelector.h"

void UAnyKeyTransitionWidget::NativeOnInitialized() { Super::NativeOnInitialized(); }

void UAnyKeyTransitionWidget::KeySelected(FInputChord SelectedKey) { FadeOut(); }

void UAnyKeyTransitionWidget::OnFadeOutEnded() {
  UE_LOG(LogTemp, Warning, TEXT("AnyKeyTransitionWidget: Fade out ended."));

  if (FadeOutEndFunc) FadeOutEndFunc();
  FadeOutEndFunc = nullptr;
}