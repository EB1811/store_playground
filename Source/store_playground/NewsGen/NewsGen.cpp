#include "NewsGen.h"
#include "HAL/Platform.h"
#include "NewsGenDataStructs.h"
#include "store_playground/NewsGen/NewsGenDataStructs.h"
#include "store_playground/Framework/GlobalDataManager.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "Kismet/GameplayStatics.h"

ANewsGen::ANewsGen() { PrimaryActorTick.bCanEverTick = false; }

void ANewsGen::BeginPlay() { Super::BeginPlay(); }

void ANewsGen::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ANewsGen::GenDaysRandomArticles(TArray<FName> GuaranteedArticles) {
  check(GlobalDataManager);

  DaysArticles.Empty();

  TArray<FArticle> EligibleArticles = GlobalDataManager->GetEligibleArticles(PublishedArticles);
  if (EligibleArticles.Num() <= 0) return;

  // * Create a layout given the article size.
  // e.g, if chosen random article is large, the rest should be smaller to fit.
  int32 TotalLayoutSpace = FMath::RandRange(6, 8);
  for (const FName& GuaranteeArticleId : GuaranteedArticles) {
    FArticle GuaranteedArticle = GlobalDataManager->GetArticle(GuaranteeArticleId);
    if (TotalLayoutSpace - NewsGenParams.ArticleSizeToSpaceMap[GuaranteedArticle.Size] < 0) continue;

    PublishedArticles.Add(GuaranteedArticle.ArticleID);
    RecentArticlesMap.Add(GuaranteedArticle.ArticleID, NewsGenParams.RecentArticlesKeepTime);
    DaysArticles.Add(GuaranteedArticle);

    EligibleArticles.RemoveAllSwap(
        [&GuaranteedArticle](const FArticle& Article) { return Article.ArticleID == GuaranteedArticle.ArticleID; });
    TotalLayoutSpace -= NewsGenParams.ArticleSizeToSpaceMap[GuaranteedArticle.Size];
  }

  EligibleArticles.RemoveAllSwap([this](const FArticle& Article) {
    return Article.AppearWeight <= 0 || RecentArticlesMap.Contains(Article.ArticleID);
  });
  if (EligibleArticles.Num() <= 0) return;

  for (TotalLayoutSpace; TotalLayoutSpace > 0; TotalLayoutSpace--) {
    TArray<FArticle> SizedArticles =
        EligibleArticles.FilterByPredicate([this, TotalLayoutSpace](const FArticle& Article) {
          return TotalLayoutSpace - NewsGenParams.ArticleSizeToSpaceMap[Article.Size] >= 0;
        });
    if (SizedArticles.Num() <= 0) break;

    FArticle SelectedArticle =
        GetWeightedRandomItem<FArticle>(EligibleArticles, [](const auto& Article) { return Article.AppearWeight; });

    PublishedArticles.Add(SelectedArticle.ArticleID);
    RecentArticlesMap.Add(SelectedArticle.ArticleID, NewsGenParams.RecentArticlesKeepTime);
    DaysArticles.Add(SelectedArticle);

    EligibleArticles.RemoveAllSwap(
        [&SelectedArticle](const FArticle& Article) { return Article.ArticleID == SelectedArticle.ArticleID; });

    TotalLayoutSpace -= NewsGenParams.ArticleSizeToSpaceMap[SelectedArticle.Size] + 1;
  }
}

void ANewsGen::TickDaysTimedVars() {
  TArray<FName> ArticlesToRemove;
  for (auto& Pair : RecentArticlesMap)
    if (Pair.Value <= 1)
      ArticlesToRemove.Add(Pair.Key);
    else
      Pair.Value--;

  for (const FName& ArticleId : ArticlesToRemove) RecentArticlesMap.Remove(ArticleId);
}