#include "NewsGen.h"
#include "HAL/Platform.h"
#include "Logging/LogVerbosity.h"
#include "NewsGenDataStructs.h"
#include "UObject/NameTypes.h"
#include "store_playground/Item/ItemBase.h"
#include "store_playground/NewsGen/NewsGenDataStructs.h"
#include "store_playground/Framework/GlobalStaticDataManager.h"
#include "store_playground/Market/Market.h"
#include "store_playground/Framework/UtilFuncs.h"
#include "store_playground/StatisticsGen/StatisticsGen.h"
#include "Kismet/GameplayStatics.h"

inline float GetItemPriceTrend(TArray<float> PriceHistory) {
  if (PriceHistory.Num() < 2) return 0.f;

  // Percentage price change fist and last.
  return (PriceHistory.Last() - PriceHistory[0]) / PriceHistory[0];
}

ANewsGen::ANewsGen() { PrimaryActorTick.bCanEverTick = false; }

void ANewsGen::BeginPlay() { Super::BeginPlay(); }

void ANewsGen::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

auto ANewsGen::GetItemPriceTrendArticle(float PriceTrend, FName ItemId) -> FArticle {
  FArticle SelectedArticle;

  constexpr float TrendThreshold = 0.15f;
  constexpr float HighTrendThreshold = 0.5f;
  if (PriceTrend > TrendThreshold) {
    TArray<FArticle> PriceIncreaseArticles =
        PriceTrend > TrendThreshold && PriceTrend <= HighTrendThreshold
            ? GlobalStaticDataManager->ArticlesArray.FilterByPredicate([](const FArticle& Article) {
                return Article.Tags.HasTag(FGameplayTag::RequestGameplayTag("Article.PriceIncrease.Mid"));
              })
            : GlobalStaticDataManager->ArticlesArray.FilterByPredicate([](const FArticle& Article) {
                return Article.Tags.HasTag(FGameplayTag::RequestGameplayTag("Article.PriceIncrease.High"));
              });
    check(PriceIncreaseArticles.Num() > 0);

    SelectedArticle = GetWeightedRandomItem<FArticle>(PriceIncreaseArticles,
                                                      [](const auto& Article) { return Article.AppearWeight; });

  } else if (PriceTrend < -TrendThreshold) {
    TArray<FArticle> PriceDecreaseArticles =
        PriceTrend < -TrendThreshold && PriceTrend >= -HighTrendThreshold
            ? GlobalStaticDataManager->ArticlesArray.FilterByPredicate([](const FArticle& Article) {
                return Article.Tags.HasTag(FGameplayTag::RequestGameplayTag("Article.PriceDecrease.Mid"));
              })
            : GlobalStaticDataManager->ArticlesArray.FilterByPredicate([](const FArticle& Article) {
                return Article.Tags.HasTag(FGameplayTag::RequestGameplayTag("Article.PriceDecrease.High"));
              });
    check(PriceDecreaseArticles.Num() > 0);

    SelectedArticle = GetWeightedRandomItem<FArticle>(PriceDecreaseArticles,
                                                      [](const auto& Article) { return Article.AppearWeight; });

  } else {
    TArray<FArticle> PriceStableArticles =
        GlobalStaticDataManager->ArticlesArray.FilterByPredicate([](const FArticle& Article) {
          return Article.Tags.HasTag(FGameplayTag::RequestGameplayTag("Article.PriceStable"));
        });
    check(PriceStableArticles.Num() > 0);

    SelectedArticle =
        GetWeightedRandomItem<FArticle>(PriceStableArticles, [](const auto& Article) { return Article.AppearWeight; });
  }

  auto ItemName = Market->AllItemsMap[ItemId]->TextData.Name;
  SelectedArticle.ArticleID = FName(*FString::Printf(TEXT("PriceTrend_%s_%s_%f"), *ItemId.ToString(),
                                                     *SelectedArticle.ArticleID.ToString(), FMath::FRand()));
  SelectedArticle.TextData.Body =
      FText::FromString(SelectedArticle.TextData.Body.ToString().Replace(TEXT("{ItemName}"), *ItemName.ToString()));

  return SelectedArticle;
}

void ANewsGen::GenDaysRandomArticles() {
  check(GlobalStaticDataManager && Market);

  DaysArticles.Empty();
  bNewArticles = true;

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
  int32 NumColumns = 3;

  for (int32 i = 0; i < NumColumns; i++) {
    int32 TotalLayoutSpace = 3;

    for (const auto& GuaranteedArticle : GuaranteedArticles) {
      if (TotalLayoutSpace - NewsGenParams.ArticleSizeToSpaceMap[GuaranteedArticle.Size] < 0) continue;

      PublishedArticles.Add(GuaranteedArticle.ArticleID);
      RecentArticlesMap.Add(GuaranteedArticle.ArticleID, NewsGenParams.RecentArticlesKeepTime);
      DaysArticles.Add(GuaranteedArticle);

      PossibleArticles.RemoveAllSwap(
          [&GuaranteedArticle](const FArticle& Article) { return Article.ArticleID == GuaranteedArticle.ArticleID; });
      TotalLayoutSpace -= NewsGenParams.ArticleSizeToSpaceMap[GuaranteedArticle.Size];
    }
    GuaranteedArticles.RemoveAllSwap([this](const FArticle& Article) {
      return PublishedArticles.ContainsByPredicate(
          [&](FName& PublishedID) { return PublishedID == Article.ArticleID; });
    });

    PossibleArticles.RemoveAllSwap([this](const FArticle& Article) {
      return Article.AppearWeight <= 0 || RecentArticlesMap.Contains(Article.ArticleID);
    });
    if (PossibleArticles.Num() <= 0) return;

    // Price trend based articles.
    int32 PriceTrendArticleNum = FMath::RandRange(0, 1);
    for (int32 j = 0; j < PriceTrendArticleNum; j++) {
      if (TotalLayoutSpace <= 0) break;

      FItemStatistics RandomItemStat =
          StatisticsGen->ItemStatisticsMap.Array()[FMath::RandRange(0, StatisticsGen->ItemStatisticsMap.Num() - 1)]
              .Value;
      float PriceTrend = GetItemPriceTrend(RandomItemStat.PriceHistory);
      UE_LOG(LogTemp, Log, TEXT("Selected item %s price trend: %f"), *RandomItemStat.ItemId.ToString(), PriceTrend);

      FArticle SelectedArticle = GetItemPriceTrendArticle(PriceTrend, RandomItemStat.ItemId);
      DaysArticles.Add(SelectedArticle);

      TotalLayoutSpace -= NewsGenParams.ArticleSizeToSpaceMap[SelectedArticle.Size];
    }

    while (TotalLayoutSpace > 0 && PossibleArticles.Num() > 0) {
      TArray<FArticle> SizedArticles =
          PossibleArticles.FilterByPredicate([this, TotalLayoutSpace](const FArticle& Article) {
            return TotalLayoutSpace - NewsGenParams.ArticleSizeToSpaceMap[Article.Size] >= 0;
          });
      if (SizedArticles.Num() <= 0) break;

      FArticle SelectedArticle =
          GetWeightedRandomItem<FArticle>(SizedArticles, [](const auto& Article) { return Article.AppearWeight; });

      PublishedArticles.Add(SelectedArticle.ArticleID);
      RecentArticlesMap.Add(SelectedArticle.ArticleID, NewsGenParams.RecentArticlesKeepTime);
      DaysArticles.Add(SelectedArticle);

      PossibleArticles.RemoveAllSwap(
          [&SelectedArticle](const FArticle& Article) { return Article.ArticleID == SelectedArticle.ArticleID; });
      TotalLayoutSpace -= NewsGenParams.ArticleSizeToSpaceMap[SelectedArticle.Size];
      UE_LOG(LogTemp, Log, TEXT("Selected article: %s, size: %d, space left: %d"),
             *SelectedArticle.ArticleID.ToString(), NewsGenParams.ArticleSizeToSpaceMap[SelectedArticle.Size],
             TotalLayoutSpace);
    }

    // Fill remaining space with price trend articles if no other articles fit.
    if (TotalLayoutSpace > 0 && PossibleArticles.Num() <= 0) {
      while (TotalLayoutSpace > 0) {
        FItemStatistics RandomItemStat =
            StatisticsGen->ItemStatisticsMap.Array()[FMath::RandRange(0, StatisticsGen->ItemStatisticsMap.Num() - 1)]
                .Value;
        float PriceTrend = GetItemPriceTrend(RandomItemStat.PriceHistory);

        FArticle SelectedArticle = GetItemPriceTrendArticle(PriceTrend, RandomItemStat.ItemId);
        DaysArticles.Add(SelectedArticle);

        TotalLayoutSpace -= NewsGenParams.ArticleSizeToSpaceMap[SelectedArticle.Size];
      }
    }
  }
}

void ANewsGen::TickDaysTimedVars() {
  TArray<FName> ArticlesToRemove;
  for (auto& Pair : RecentArticlesMap)
    if (Pair.Value <= 1) ArticlesToRemove.Add(Pair.Key);
    else Pair.Value--;

  for (const FName& ArticleId : ArticlesToRemove) RecentArticlesMap.Remove(ArticleId);
}