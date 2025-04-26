#include "StorePhaseTransitionWidget.h"

void UStorePhaseTransitionWidget::OnFadeInEnded() {
  if (FadeInEndFunc) FadeInEndFunc();
}
void UStorePhaseTransitionWidget::OnFadeOutEnded() {
  if (FadeOutEndFunc) FadeOutEndFunc();
}