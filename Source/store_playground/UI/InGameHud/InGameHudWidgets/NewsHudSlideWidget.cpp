#include "NewsHudSlideWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UNewsHudSlideWidget::RefreshUI() {
  bool bNewNews = true;  // Temp: replace with actual news check logic.

  if (bNewNews) {
    NotificationText->SetText(FText::FromString("!"));
    // NotificationImage->SetVisibility(ESlateVisibility::Visible);
  } else {
    NotificationText->SetText(FText::FromString(""));
    // NotificationImage->SetVisibility(ESlateVisibility::Collapsed);
  }
}
