#include "NewspaperArticleWidget.h"
#include "store_playground/NewsGen/NewsGenDataStructs.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UNewspaperArticleWidget::SetArticleData(const FArticle& ArticleData) {
  ArticleImage->SetBrushFromTexture(ArticleData.AssetData.Picture);
  ArticleTitleText->SetText(ArticleData.TextData.Title);
  ArticleBodyText->SetText(ArticleData.TextData.Body);
}
