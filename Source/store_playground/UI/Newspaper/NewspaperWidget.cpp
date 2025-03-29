#include "NewspaperWidget.h"
#include "store_playground/UI/Newspaper/NewspaperArticleWidget.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UNewspaperWidget::RefreshNewspaperUI() {
  check(NewsGenRef && NewspaperArticleClass);

  NewspaperTitleText->SetText(FText::FromString("Daily News"));  // Temp.

  NewspaperPanelWrapBox->ClearChildren();
  for (const FArticle& Article : NewsGenRef->DaysArticles) {
    if (UNewspaperArticleWidget* ArticleWidget = CreateWidget<UNewspaperArticleWidget>(this, NewspaperArticleClass)) {
      ArticleWidget->SetArticleData(Article);
      NewspaperPanelWrapBox->AddChildToWrapBox(ArticleWidget);
    }
  }
}