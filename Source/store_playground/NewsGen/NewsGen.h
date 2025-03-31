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

UCLASS(Blueprintable)
class STORE_PLAYGROUND_API ANewsGen : public AInfo {
  GENERATED_BODY()

public:
  ANewsGen();

  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, Category = "NewsGen")
  const class AGlobalDataManager* GlobalDataManager;

  UPROPERTY(EditAnywhere, Category = "NewsGen")
  FNewsGenParams NewsGenParams;

  UPROPERTY(EditAnywhere, Category = "NewsGen")
  TArray<FName> PublishedArticles;
  UPROPERTY(EditAnywhere, Category = "NewsGen")
  TMap<FName, int32> RecentArticlesMap;

  UPROPERTY(EditAnywhere, Category = "NewsGen")
  TArray<FArticle> DaysArticles;

  void GenDaysRandomArticles(TArray<FName> GuaranteedArticles = {});

  void TickDaysTimedVars();
};