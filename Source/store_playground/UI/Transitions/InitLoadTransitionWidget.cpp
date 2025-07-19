#include "InitLoadTransitionWidget.h"

void UInitLoadTransitionWidget::OnFadeOutEnded() {
  if (FadeOutEndFunc) FadeOutEndFunc();
}