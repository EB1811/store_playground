// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "store_playground/NewsGen/NewsGenDataStructs.h"
#include "NewsGen.generated.h"

USTRUCT()
struct FNewsGenParams {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  int32 RecentArticlesKeepTime;  // * How many days to keep the articles in the recent list.
  UPROPERTY(EditAnywhere)
  TMap<EArticleSize, int32> ArticleSizeToSpaceMap;  // * The space each size takes in the layout.
};

USTRUCT()
struct FPrevArticles {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FName> PublishedIDs;
  UPROPERTY(EditAnywhere, SaveGame)
  TArray<FArticle> PriceTrendArticles;  // Since they have dynamic content and ids.
};

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ANewsGen : public AInfo {
  GENERATED_BODY()

public:
  ANewsGen();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "NewsGen")
  const class AGlobalStaticDataManager* GlobalStaticDataManager;
  UPROPERTY(EditAnywhere, Category = "NewsGen")
  const class ADayManager* DayManager;
  UPROPERTY(EditAnywhere, Category = "NewsGen")
  const class AMarket* Market;
  UPROPERTY(EditAnywhere, Category = "NewsGen")
  const class AStatisticsGen* StatisticsGen;

  UPROPERTY(EditAnywhere, Category = "NewsGen")
  FNewsGenParams NewsGenParams;

  UPROPERTY(EditAnywhere, Category = "NewsGen", SaveGame)
  TArray<FName> PublishedArticles;
  UPROPERTY(EditAnywhere, Category = "NewsGen", SaveGame)
  TMap<FName, int32> RecentArticlesMap;
  UPROPERTY(EditAnywhere, Category = "NewsGen", SaveGame)
  TMap<int32, FPrevArticles> PrevDaysArticlesMap;  // * Day to articles.

  UPROPERTY(EditAnywhere, Category = "NewsGen", SaveGame)
  TArray<FArticle> DaysArticles;
  UPROPERTY(EditAnywhere, Category = "NewsGen", SaveGame)
  bool bNewArticles;

  auto GetItemPriceTrendArticle(float PriceTrend, FName ItemId) -> FArticle;
  auto GetDaysArticles(int32 Day) -> TArray<FArticle>;

  void GenDaysRandomArticles();

  void TickDaysTimedVars();
};