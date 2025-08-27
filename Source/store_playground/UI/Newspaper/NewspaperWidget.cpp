#include "NewspaperWidget.h"
#include "store_playground/UI/Newspaper/ArticleWidget.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/DayManager/DayManager.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

inline auto GetArticleSizeInt(EArticleSize Size) -> int32 {
  switch (Size) {
    case EArticleSize::Small: return 1;
    case EArticleSize::Medium: return 2;
    case EArticleSize::Large: return 3;
    default: checkNoEntry(); return 0;
  }
}

void UNewspaperWidget::RefreshUI() {
  DayText->SetText(FText::FromString(FString::Printf(TEXT("Day %d"), DayManager->CurrentDay)));

  // Large article = whole column. Prioritise being in the middle.
  // Medium article = 2/3 of column.
  // Small article = 1/3 of column.
  // Total 3 columns.
  // Prioritise bigger articles. Smaller ones can be skipped if there is not enough space.

  TArray<FArticle> Articles = NewsGen->DaysArticles;
  Articles.Sort(
      [](const FArticle& A, const FArticle& B) { return GetArticleSizeInt(A.Size) > GetArticleSizeInt(B.Size); });
  int32 LargeArticleCount =
      NewsGen->DaysArticles
          .FilterByPredicate([](const FArticle& Article) { return Article.Size == EArticleSize::Large; })
          .Num();

  TArray<FArticle> Col1Articles = {};
  {
    int32 ColumnSpace = 3;
    for (int32 i = 0; i < Articles.Num(); i++) {
      if (ColumnSpace <= 0) break;

      const FArticle& Article = Articles[i];
      int32 Size = GetArticleSizeInt(Article.Size);
      if (Size > ColumnSpace) continue;

      if (Article.Size == EArticleSize::Large && LargeArticleCount == 1) continue;  // * Go to middle column.

      Col1Articles.Add(Article);
      ColumnSpace -= Size;
    }
  }
  Articles = Articles.FilterByPredicate([&Col1Articles](const FArticle& Article) {
    return !Col1Articles.ContainsByPredicate(
        [&](const FArticle& ColArticle) { return ColArticle.ArticleID == Article.ArticleID; });
  });
  TArray<FArticle> Col2Articles = {};
  {
    int32 ColumnSpace = 3;
    if (LargeArticleCount > 0) {
      for (int32 i = 0; i < Articles.Num(); i++) {
        const FArticle& Article = Articles[i];
        if (Article.Size == EArticleSize::Large) {
          Col2Articles.Add(Article);
          ColumnSpace -= GetArticleSizeInt(Article.Size);
          break;
        }
      }
    }
    for (int32 i = 0; i < Articles.Num(); i++) {
      if (ColumnSpace <= 0) break;

      const FArticle& Article = Articles[i];
      int32 Size = GetArticleSizeInt(Article.Size);
      if (Size > ColumnSpace) continue;

      Col2Articles.Add(Article);
      ColumnSpace -= Size;
    }
  }
  Articles = Articles.FilterByPredicate([&Col2Articles](const FArticle& Article) {
    return !Col2Articles.ContainsByPredicate(
        [&](const FArticle& ColArticle) { return ColArticle.ArticleID == Article.ArticleID; });
  });
  TArray<FArticle> Col3Articles = {};
  {
    int32 ColumnSpace = 3;
    for (int32 i = 0; i < Articles.Num(); i++) {
      if (ColumnSpace <= 0) break;

      const FArticle& Article = Articles[i];
      int32 Size = GetArticleSizeInt(Article.Size);
      if (Size > ColumnSpace) continue;

      Col3Articles.Add(Article);
      ColumnSpace -= Size;
    }
  }
  Articles = Articles.FilterByPredicate([&Col3Articles](const FArticle& Article) {
    return !Col3Articles.ContainsByPredicate(
        [&](const FArticle& ColArticle) { return ColArticle.ArticleID == Article.ArticleID; });
  });

  // * Check that all articles are accounted for.
  check(Col1Articles.Num() + Col2Articles.Num() + Col3Articles.Num() == NewsGen->DaysArticles.Num());
  check(Col1Articles.Num() <= 3 && Col2Articles.Num() <= 3 && Col3Articles.Num() <= 3);
  check(Articles.Num() == 0);

  TArray<FArticle> AllArticles = {};
  AllArticles.Append(Col1Articles);
  AllArticles.Append(Col2Articles);
  AllArticles.Append(Col3Articles);

  NewspaperPanelWrapBox->ClearChildren();
  for (const FArticle& Article : AllArticles) {
    switch (Article.Size) {
      case EArticleSize::Large: {
        UArticleWidget* ArticleWidget = CreateWidget<UArticleWidget>(this, LargeArticleWidgetClass);
        check(ArticleWidget);

        ArticleWidget->Title->SetText(Article.TextData.Title);
        ArticleWidget->Text->SetText(Article.TextData.Body);
        if (Article.AssetData.Picture) ArticleWidget->Picture->SetBrushFromMaterial(Article.AssetData.Picture);

        NewspaperPanelWrapBox->AddChildToWrapBox(ArticleWidget);
        break;
      }
      case EArticleSize::Medium: {
        UArticleWidget* ArticleWidget = CreateWidget<UArticleWidget>(this, MediumArticleWidgetClass);
        check(ArticleWidget);

        ArticleWidget->Title->SetText(Article.TextData.Title);
        ArticleWidget->Text->SetText(Article.TextData.Body);
        if (Article.AssetData.Picture) ArticleWidget->Picture->SetBrushFromMaterial(Article.AssetData.Picture);

        NewspaperPanelWrapBox->AddChildToWrapBox(ArticleWidget);
        break;
      }
      case EArticleSize::Small: {
        UArticleWidget* ArticleWidget = CreateWidget<UArticleWidget>(this, SmallArticleWidgetClass);
        check(ArticleWidget);

        ArticleWidget->Title->SetText(Article.TextData.Title);
        ArticleWidget->Text->SetText(Article.TextData.Body);
        if (Article.AssetData.Picture) ArticleWidget->Picture->SetBrushFromMaterial(Article.AssetData.Picture);

        NewspaperPanelWrapBox->AddChildToWrapBox(ArticleWidget);
        break;
      }
      default: checkNoEntry();
    }
  }

  if (NewsGen->bNewArticles) NewsGen->bNewArticles = false;
}

void UNewspaperWidget::InitUI(const ADayManager* _DayManager, ANewsGen* _NewsGen) {
  check(_DayManager && _NewsGen);

  DayManager = _DayManager;
  NewsGen = _NewsGen;
}