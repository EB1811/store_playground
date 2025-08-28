#include "NewspaperWidget.h"
#include "store_playground/UI/Newspaper/ArticleWidget.h"
#include "store_playground/NewsGen/NewsGen.h"
#include "store_playground/DayManager/DayManager.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

inline FString JustifyLine(const TArray<FString>& Words, int32 LineWidth, bool IsLastLine) {
  if (Words.Num() == 0) return TEXT("");
  if (Words.Num() == 1 || IsLastLine) return FString::Join(Words, TEXT(" "));

  // Calculate how many extra spaces we need to distribute
  int32 BaseTextLength = 0;
  for (const FString& Word : Words) BaseTextLength += Word.Len();

  int32 MinSpaces = Words.Num() - 1;
  int32 TotalSpacesNeeded = LineWidth - BaseTextLength;
  int32 ExtraSpaces = TotalSpacesNeeded - MinSpaces;
  if (ExtraSpaces <= 0) return FString::Join(Words, TEXT(" "));

  int32 GapsCount = Words.Num() - 1;
  TArray<int32> SpacesPerGap;
  for (int32 i = 0; i < GapsCount; i++) SpacesPerGap.Add(1);

  // Distribute extra spaces alternating from left and right
  bool AddFromLeft = true;
  int32 RemainingSpaces = ExtraSpaces;
  while (RemainingSpaces > 0) {
    if (AddFromLeft) {
      for (int32 i = 0; i < GapsCount && RemainingSpaces > 0; i++) {
        SpacesPerGap[i]++;
        RemainingSpaces--;
      }
    } else {
      for (int32 i = GapsCount - 1; i >= 0 && RemainingSpaces > 0; i--) {
        SpacesPerGap[i]++;
        RemainingSpaces--;
      }
    }
    AddFromLeft = !AddFromLeft;
  }

  // Build the justified line
  FString Result = Words[0];
  for (int32 i = 1; i < Words.Num(); i++) {
    for (int32 j = 0; j < SpacesPerGap[i - 1]; j++) Result += TEXT(" ");
    Result += Words[i];
  }

  return Result;
}
inline FString JustifyText(const FString& Text, int32 LineWidth) {
  TArray<FString> Words;
  Text.ParseIntoArray(Words, TEXT(" "), true);

  if (Words.Num() == 0) return Text;

  TArray<FString> JustifiedLines;
  TArray<FString> CurrentLine;
  int32 CurrentLineLength = 0;

  // Build lines that fit within the specified width
  for (const FString& Word : Words) {
    int32 WordLength =
        Word.Replace(TEXT("\n"), TEXT("")).Replace(TEXT("\\"), TEXT("")).Replace(TEXT("'"), TEXT("")).Len();

    if (CurrentLine.Num() > 0 && (CurrentLineLength + 1 + WordLength) > LineWidth) {
      FString JustifiedLine = JustifyLine(CurrentLine, LineWidth, false);
      JustifiedLines.Add(JustifiedLine);

      CurrentLine.Empty();
      CurrentLine.Add(Word);
      CurrentLineLength = WordLength;
    } else {
      CurrentLine.Add(Word);
      CurrentLineLength += (CurrentLine.Num() > 1 ? 1 : 0) + WordLength;  // +1 for space
    }
  }

  // Handle the last line (don't justify the last line)
  if (CurrentLine.Num() > 0) {
    FString LastLine = FString::Join(CurrentLine, TEXT(" "));
    JustifiedLines.Add(LastLine);
  }

  return FString::Join(JustifiedLines, TEXT("  "));
}
inline void SetArticleBody(UArticleWidget* Widget, const FString& BodyText, float DefaultWidth = 440.0f) {
  float BodyWidth = Widget->Text->GetDesiredSize().X > 0 ? Widget->Text->GetDesiredSize().X : DefaultWidth;
  float FontSize = Widget->Text->GetFont().Size;

  // Rough estimate of characters that can fit in a line with 52% of font size width.
  int32 EstimatedLineWidth = (int32)((BodyWidth) / (FontSize * 0.52f));
  FString JustifiedText = JustifyText(BodyText, EstimatedLineWidth);

  Widget->Text->SetText(FText::FromString(JustifiedText));
}

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
        SetArticleBody(ArticleWidget, Article.TextData.Body.ToString(), 460.0f);
        if (Article.AssetData.Picture) ArticleWidget->Picture->SetBrushFromMaterial(Article.AssetData.Picture);

        NewspaperPanelWrapBox->AddChildToWrapBox(ArticleWidget);
        break;
      }
      case EArticleSize::Medium: {
        UArticleWidget* ArticleWidget = CreateWidget<UArticleWidget>(this, MediumArticleWidgetClass);
        check(ArticleWidget);

        ArticleWidget->Title->SetText(Article.TextData.Title);
        SetArticleBody(ArticleWidget, Article.TextData.Body.ToString());
        if (Article.AssetData.Picture) ArticleWidget->Picture->SetBrushFromMaterial(Article.AssetData.Picture);

        NewspaperPanelWrapBox->AddChildToWrapBox(ArticleWidget);
        break;
      }
      case EArticleSize::Small: {
        UArticleWidget* ArticleWidget = CreateWidget<UArticleWidget>(this, SmallArticleWidgetClass);
        check(ArticleWidget);

        ArticleWidget->Title->SetText(Article.TextData.Title);
        SetArticleBody(ArticleWidget, Article.TextData.Body.ToString());
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