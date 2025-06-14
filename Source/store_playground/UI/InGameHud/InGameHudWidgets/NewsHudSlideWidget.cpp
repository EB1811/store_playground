#include "NewsHudSlideWidget.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UNewsHudSlideWidget::RefreshUI() {
  if (NewsGen->bNewArticles) {
    NotificationText->SetText(FText::FromString("!"));
    // NotificationImage->SetVisibility(ESlateVisibility::Visible);
  } else {
    NotificationText->SetText(FText::FromString(""));
    // NotificationImage->SetVisibility(ESlateVisibility::Collapsed);
  }
}

void UNewsHudSlideWidget::InitUI(const ANewsGen* _NewsGen) {
  check(_NewsGen);

  NewsGen = _NewsGen;
}
