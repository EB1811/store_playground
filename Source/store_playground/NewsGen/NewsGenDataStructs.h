// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NewsGenDataStructs.generated.h"

UENUM()
enum class EArticleSize : uint8 {
  Small UMETA(DisplayName = "Small"),
  Medium UMETA(DisplayName = "Medium"),
  Large UMETA(DisplayName = "Large"),
};

USTRUCT()
struct FArticleTextData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FText Title;
  UPROPERTY(EditAnywhere)
  FText Body;
};
USTRUCT()
struct FArticleAssetData {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  class UTexture2D* Picture;
};

USTRUCT()
struct FArticle {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ArticleID;

  UPROPERTY(EditAnywhere)
  FName RequirementsFilter;
  UPROPERTY(EditAnywhere)
  int32 AppearWeight;  // * 0 = never appears
  UPROPERTY(EditAnywhere)
  bool bIsRepeatable;

  UPROPERTY(EditAnywhere)
  EArticleSize Size;
  UPROPERTY(EditAnywhere)
  FArticleTextData TextData;
  UPROPERTY(EditAnywhere)
  FArticleAssetData AssetData;
};
USTRUCT()
struct FArticleRow : public FTableRowBase {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere)
  FName ArticleID;

  UPROPERTY(EditAnywhere)
  FName RequirementsFilter;
  UPROPERTY(EditAnywhere)
  int32 AppearWeight;  // * 0 = never appears
  UPROPERTY(EditAnywhere)
  bool bIsRepeatable;

  UPROPERTY(EditAnywhere)
  EArticleSize Size;
  UPROPERTY(EditAnywhere)
  FArticleTextData TextData;
  UPROPERTY(EditAnywhere)
  FArticleAssetData AssetData;
};