#include "NewspaperArticleWidget.h"
#include "Logging/LogVerbosity.h"
#include "store_playground/NewsGen/NewsGenDataStructs.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Materials/MaterialInstance.h"

void UNewspaperArticleWidget::SetArticleData(const FArticle& ArticleData) {
  ArticleImage->SetBrushFromMaterial(ArticleData.AssetData.Picture);
  ArticleTitleText->SetText(ArticleData.TextData.Title);
  ArticleBodyText->SetText(ArticleData.TextData.Body);
}