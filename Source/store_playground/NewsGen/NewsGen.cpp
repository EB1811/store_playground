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

void ANewsGen::GenDaysRandomArticles(FName ArticleId) {
  check(GlobalDataManager);

  DaysArticles.Empty();

  const TMap<EReqFilterOperand, std::any> GameDataMap = {{}};  // TODO: Get game data.
  TArray<FArticle> EligibleArticles = GlobalDataManager->GetEligibleArticles(GameDataMap, PublishedArticles);
  if (EligibleArticles.Num() <= 0) return;

  // * Create a layout given the article size.
  // e.g, if chosen random article is large, the rest should be smaller to fit.
  int32 TotalLayoutSpace = FMath::RandRange(6, 8);
  if (ArticleId != NAME_None) {
    FArticle GuaranteedArticle = GlobalDataManager->GetArticle(ArticleId);

    PublishedArticles.Add(GuaranteedArticle.ArticleID);
    RecentArticles.Add(GuaranteedArticle.ArticleID);
    DaysArticles.Add(GuaranteedArticle);

    EligibleArticles.RemoveAllSwap(
        [&GuaranteedArticle](const FArticle& Article) { return Article.ArticleID == GuaranteedArticle.ArticleID; });
    TotalLayoutSpace -= NewsGenParams.ArticleSizeToSpaceMap[GuaranteedArticle.Size];
  }

  for (TotalLayoutSpace; TotalLayoutSpace > 0; TotalLayoutSpace--) {
    TArray<FArticle> SizedArticles =
        EligibleArticles.FilterByPredicate([this, TotalLayoutSpace](const FArticle& Article) {
          return TotalLayoutSpace - NewsGenParams.ArticleSizeToSpaceMap[Article.Size] >= 0;
        });
    if (SizedArticles.Num() <= 0) break;

    FArticle SelectedArticle =
        GetWeightedRandomItem<FArticle>(EligibleArticles, [](const auto& Article) { return Article.AppearWeight; });

    PublishedArticles.Add(SelectedArticle.ArticleID);
    RecentArticles.Add(SelectedArticle.ArticleID);
    DaysArticles.Add(SelectedArticle);

    EligibleArticles.RemoveAll(
        [&SelectedArticle](const FArticle& Article) { return Article.ArticleID == SelectedArticle.ArticleID; });

    TotalLayoutSpace -= NewsGenParams.ArticleSizeToSpaceMap[SelectedArticle.Size] + 1;
  }
}