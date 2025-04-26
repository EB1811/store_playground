#include "LevelLoadingTransitionWidget.h"

void ULevelLoadingTransitionWidget::OnFadeInEnded() {
  if (FadeInEndFunc) FadeInEndFunc();
}
void ULevelLoadingTransitionWidget::OnFadeOutEnded() {
  if (FadeOutEndFunc) FadeOutEndFunc();
}