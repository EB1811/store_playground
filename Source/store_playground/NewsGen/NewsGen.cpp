#include "NewsGen.h"
#include "HAL/Platform.h"
#include "NewsGenDataStructs.h"
#include "store_playground/NewsGen/NewsGenDataStructs.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "Kismet/GameplayStatics.h"

ANewsGen::ANewsGen() { PrimaryActorTick.bCanEverTick = false; }

void ANewsGen::BeginPlay() { Super::BeginPlay(); }

void ANewsGen::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void ANewsGen::GenDaysRandomArticles() {
  check(GlobalStaticDataManager && Market);

  DaysArticles.Empty();

  TArray<FArticle> GuaranteedArticles = {};
  TArray<FArticle> PossibleArticles = GlobalStaticDataManager->GetEligibleGeneralArticles(PublishedArticles);
  check(PossibleArticles.Num() > 0);

  for (const auto& EconEvent : Market->TodaysEconEvents) {
    if (!EconEvent.ArticleID.IsNone()) {
      const auto Article = GlobalStaticDataManager->GetArticle(EconEvent.ArticleID);
      GuaranteedArticles.Add(Article);
    }

    TArray<FArticle> Articles = GlobalStaticDataManager->GetEligibleSpecialArticles(PublishedArticles, EconEvent.Tags);
    if (Articles.Num() <= 0) continue;

    switch (EconEvent.Severity) {
      case EEconEventSeverity::Minor:
      case EEconEventSeverity::Major: PossibleArticles.Append(Articles); break;
      case EEconEventSeverity::Catastrophic: GuaranteedArticles.Append(Articles); break;
      default: checkNoEntry();
    }
  }

  // * Create a layout given the article size.
  // e.g, if chosen random article is large, the rest should be smaller to fit.
  int32 TotalLayoutSpace = FMath::RandRange(6, 8);

  for (const auto& GuaranteedArticle : GuaranteedArticles) {
    if (TotalLayoutSpace - NewsGenParams.ArticleSizeToSpaceMap[GuaranteedArticle.Size] < 0) continue;

    PublishedArticles.Add(GuaranteedArticle.ArticleID);
    RecentArticlesMap.Add(GuaranteedArticle.ArticleID, NewsGenParams.RecentArticlesKeepTime);
    DaysArticles.Add(GuaranteedArticle);

    PossibleArticles.RemoveAllSwap(
        [&GuaranteedArticle](const FArticle& Article) { return Article.ArticleID == GuaranteedArticle.ArticleID; });
    TotalLayoutSpace -= NewsGenParams.ArticleSizeToSpaceMap[GuaranteedArticle.Size];
  }

  PossibleArticles.RemoveAllSwap([this](const FArticle& Article) {
    return Article.AppearWeight <= 0 || RecentArticlesMap.Contains(Article.ArticleID);
  });
  if (PossibleArticles.Num() <= 0) return;

  for (TotalLayoutSpace; TotalLayoutSpace > 0; TotalLayoutSpace--) {
    TArray<FArticle> SizedArticles =
        PossibleArticles.FilterByPredicate([this, TotalLayoutSpace](const FArticle& Article) {
          return TotalLayoutSpace - NewsGenParams.ArticleSizeToSpaceMap[Article.Size] >= 0;
        });
    if (SizedArticles.Num() <= 0) break;

    FArticle SelectedArticle =
        GetWeightedRandomItem<FArticle>(PossibleArticles, [](const auto& Article) { return Article.AppearWeight; });

    PublishedArticles.Add(SelectedArticle.ArticleID);
    RecentArticlesMap.Add(SelectedArticle.ArticleID, NewsGenParams.RecentArticlesKeepTime);
    DaysArticles.Add(SelectedArticle);

    PossibleArticles.RemoveAllSwap(
        [&SelectedArticle](const FArticle& Article) { return Article.ArticleID == SelectedArticle.ArticleID; });

    TotalLayoutSpace -= NewsGenParams.ArticleSizeToSpaceMap[SelectedArticle.Size] + 1;
  }
}

void ANewsGen::TickDaysTimedVars() {
  TArray<FName> ArticlesToRemove;
  for (auto& Pair : RecentArticlesMap)
    if (Pair.Value <= 1) ArticlesToRemove.Add(Pair.Key);
    else Pair.Value--;

  for (const FName& ArticleId : ArticlesToRemove) RecentArticlesMap.Remove(ArticleId);
}